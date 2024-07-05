#include "gui.hpp"
#include "duck-lisp.hpp"
#include "duck-lisp/duckVM.h"
#include "log.h"
#include <cstddef>
#include "file_utilities.hpp"


dl_error_t gui_callback_makeInstance(duckVM_t *duckVM);
dl_error_t gui_callback_getMember(duckVM_t *duckVM);
dl_error_t gui_callback_setMember(duckVM_t *duckVM);
dl_error_t gui_generator_present(duckLisp_t*, duckLisp_compileState_t *, dl_array_t*, duckLisp_ast_expression_t*);


Gui::Gui(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp) {
	// Slightly less global than global. Only this VM instance can see this object.
	duckLisp->setUserDataByName("gui", this);
	duckVM->setUserDataByName("gui", this);
	duckVM->setUserDataByName("duck-lisp", duckLisp.get());  // VM needs access to symbol trie.
	duckLisp->registerGenerator("present", gui_generator_present, "(L)");
	registerCallback(duckVM, duckLisp, "gui-make-instance", "(I)", gui_callback_makeInstance);
	registerCallback(duckVM, duckLisp, "gui-get-member", "(I I)", gui_callback_getMember);
	registerCallback(duckVM, duckLisp, "gui-set-member", "(I I I)", gui_callback_setMember);
}

void Gui::render(RenderWindow *window) {
	// TODO: Traverse linked list, not vector itself. Will preserve render order. This will also make it so free objects
	//       do not have to be skipped over.
	size_t objectPool_length = objectPool.size();
	for (size_t object_index = 0; object_index < objectPool_length; object_index++) {
		GuiObject &object = objectPool[object_index];
		switch (object.type) {
		case GuiObjectType_free: break;
		case GuiObjectType_image: {
			auto &image = object.image;
			// Load texture if not already loaded.
			if (image.texture == nullptr && image.file != "") {
				info("Loading image \"" + image.file + "\".");
				auto *texture = window->loadTexture(image.file.c_str());
				if (texture == nullptr) {
					warning("Failed to load image \"" + image.file + "\".");
					break;
				}
				image.texture = texture;
			}
			if (!object.visible) break;
			window->render(image.texture, image.rect);
			break;
		}
		case GuiObjectType_window:
			// window->setBackground(object.window.backgroundColor);
			window->clear();
			break;
		default:
			error("Invalid GuiObject type " + std::to_string(object.type) + ".");
		}
	}
}


GuiObject::GuiObject(GuiObjectType type) {
	this->type = type;
	if (type == GuiObjectType_window) {
		this->window = GuiWidgetWindow();
	}
}

size_t Gui::allocateObject(GuiObjectType type) {
	if (freeList.empty()) {
		objectPool.push_back(GuiObject(type));
		return objectPool.size() - 1;
	}
	else {
		size_t index = freeList.back();
		freeList.pop_back();
		return index;
	}
}

void Gui::freeObject(size_t objectIndex) {
	freeList.push_back(objectIndex);
}

GuiObject Gui::getObject(size_t objectIndex) {
	return objectPool[objectIndex];
}

void Gui::setObject(size_t objectIndex, GuiObject object) {
	objectPool[objectIndex] = object;
}

std::string getNameFromType(const GuiObjectType typeName) {
	// If-else because I don't want to deal with a map.
	if (GuiObjectType_window == typeName) {
		return "window";
	}
	else if (GuiObjectType_image == typeName) {
		return "image";
	}
	return "invalid";
}

GuiObjectType getTypeFromName(const std::string typeName) {
	// Indexing type by name instead of integer is harder on C++, but this way the enumeration can't get out of sync
	// with DL.
	// If-else because I don't want to deal with a map.
	if ("window" == typeName) {
		return GuiObjectType_window;
	}
	else if ("image" == typeName) {
		return GuiObjectType_image;
	}
	return GuiObjectType_invalid;
}



/* Functions for use by duck-lisp. */

dl_error_t guiObject_destructor(duckVM_gclist_t *gclist, duckVM_object_t *userObject) {
	dl_error_t e = dl_error_ok;

	duckVM_t *duckVM = gclist->duckVM;
	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));
	size_t index = (size_t) userObject;
	(void) gui->freeObject(index);
	debug("GuiObject: DESTRUCT");

	return e;
}


// gui-make-instance type-name::String
dl_error_t gui_callback_makeInstance(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));

	// Fetch type name.
	dl_bool_t isString;
	e = duckVM_isString(duckVM, &isString);
	if (e) return e;
	if (!isString) {
		error("First argument of `gui-make-instance` must be a string.");
		return dl_error_invalidValue;
	}
	dl_uint8_t *cTypeName;
	dl_size_t cTypeName_length;
	e = duckVM_copyString(duckVM, &cTypeName, &cTypeName_length);
	if (e) return e;
	e = duckVM_pop(duckVM);
	if (e) return e;

	size_t objectIndex = gui->allocateObject(getTypeFromName(std::string((char *) cTypeName, cTypeName_length)));
	e = DL_FREE(duckVM->memoryAllocation, &cTypeName);
	if (e) return e;

	// Store the index in the pointer value.
	duckVM_object_t userObject = duckVM_object_makeUser((void *) objectIndex, nullptr, guiObject_destructor);
	e = duckVM_object_push(duckVM, &userObject);
	if (e) return e;

	return e;
}

// Stack is empty.
dl_error_t GuiObject::pushVisible(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;
	// stack:

	e = duckVM_pushBoolean(duckVM);
	if (e) return e;
	// stack: false
	e = duckVM_setBoolean(duckVM, visible);
	if (e) return e;
	// stack: visible

	return e;
}

// The value to set the field "value" to is the top object in the `duckVM` stack. Do not pop that object off. Only copy.
dl_error_t GuiObject::setVisible(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;
	// stack:

	{
		dl_bool_t isBoolean;
		e = duckVM_isBoolean(duckVM, &isBoolean);
		if (e) return e;
		if (!isBoolean) {
			error("\"visible\" field may only be set to a boolean.");
			return dl_error_invalidValue;
		}
	}
	{
		dl_bool_t boolean;
		e = duckVM_copyBoolean(duckVM, &boolean);
		if (e) return e;
		visible = boolean;
	}

	return e;
}

// gui-get-member object::GuiObject name::String
dl_error_t gui_callback_getMember(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;
	// stack: object name value

	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));

	duckVM_object_type_t type;
	e = duckVM_typeOf(duckVM, &type);
	if (e) return e;

	e = duckVM_push(duckVM, -2);
	if (e) return e;
	// stack: object name object
	{
		dl_bool_t isUser;
		e = duckVM_isUser(duckVM, &isUser);
		if (e) return e;
		if (!isUser) {
			error("First argument of `gui-get-member` should be a GuiObject.");
			return dl_error_invalidValue;
		}
	}
	dl_size_t guiObject_index;
	{
		duckVM_object_t userObject;
		e = duckVM_object_pop(duckVM, &userObject);
		if (e) return e;
		guiObject_index = (size_t) userObject.value.user.data;
	}
	// stack: object name

	// Assume it exists because we have garbage collection. Use-after-free should be harder to commit.
	GuiObject object = gui->getObject(guiObject_index);

	{
		dl_bool_t isString;
		e = duckVM_isString(duckVM, &isString);
		if (e) return e;
		if (!isString) {
			error("Second argument of `gui-get-member` should be a string.");
			return dl_error_invalidValue;
		}
	}
	dl_uint8_t *cName = nullptr;
	dl_size_t cName_length;
	e = duckVM_copyString(duckVM, &cName, &cName_length);
	if (e) return e;
	std::string name = std::string((char *) cName, cName_length);
	e = DL_FREE(duckVM->memoryAllocation, &cName);
	if (e) return e;

	duckVM_popSeveral(duckVM, 2);
	// stack:

	switch (object.type) {
	case GuiObjectType_window:
		e = object.window.pushMember(duckVM, name);
		if (e) return e;
		break;
	case GuiObjectType_image:
		if (name == "visible") {
			e = object.pushVisible(duckVM);
			if (e) return e;
			break;
		}
		e = object.image.pushMember(duckVM, name);
		if (e) return e;
		break;
	default:
		error("Unsupported GuiObject type.");
		return dl_error_invalidValue;
	}
	// stack: value

	return e;
}

// gui-set-member object::GuiObject name::String value::Any
dl_error_t gui_callback_setMember(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;
	// stack: object name value

	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));

	duckVM_object_type_t type;
	e = duckVM_typeOf(duckVM, &type);
	if (e) return e;

	e = duckVM_push(duckVM, -3);
	if (e) return e;
	// stack: object name value object
	{
		dl_bool_t isUser;
		e = duckVM_isUser(duckVM, &isUser);
		if (e) return e;
		if (!isUser) {
			error("First argument of `gui-set-member` should be a GuiObject.");
			return dl_error_invalidValue;
		}
	}
	size_t guiObject_index;
	{
		duckVM_object_t userObject;
		e = duckVM_object_pop(duckVM, &userObject);
		if (e) return e;
		guiObject_index = (size_t) userObject.value.user.data;
	}
	// stack: object name value

	// Assume it exists because we have garbage collection. Use-after-free should be harder to commit.
	GuiObject object = gui->getObject(guiObject_index);

	e = duckVM_push(duckVM, -2);
	if (e) return e;
	// stack: object name value name
	{
		dl_bool_t isString;
		e = duckVM_isString(duckVM, &isString);
		if (e) return e;
		if (!isString) {
			error("Second argument of `gui-set-member` should be a string.");
			return dl_error_invalidValue;
		}
	}
	dl_uint8_t *cName = nullptr;
	dl_size_t cName_length;
	e = duckVM_copyString(duckVM, &cName, &cName_length);
	if (e) return e;
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: object name value
	std::string name = std::string((char *) cName, cName_length);
	e = DL_FREE(duckVM->memoryAllocation, &cName);
	if (e) return e;

	switch (object.type) {
	case GuiObjectType_window:
		e = object.window.setMember(duckVM, name);
		if (e) return e;
		break;
	case GuiObjectType_image:
		if (name == "visible") {
			e = object.setVisible(duckVM);
			if (e) return e;
			break;
		}
		e = object.image.setMember(duckVM, name);
		if (e) return e;
		break;
	default:
		error("Unsupported GuiObject type.");
		return dl_error_invalidValue;
	}

	(void) gui->setObject(guiObject_index, object);

	e = duckVM_copyFromTop(duckVM, -3);
	if (e) return e;
	// stack: value name value
	e = duckVM_popSeveral(duckVM, 2);
	if (e) return e;
	// stack: value

	return e;
}


dl_error_t getGlobalType(duckVM_t *duckVM, std::string globalName, dl_size_t *type) {
	dl_error_t e = dl_error_ok;
	// stack:

	// Got the composite, but we don't know what type it should be. Find the value of "(object)-type".
	auto *duckLisp = static_cast<duckLisp_t *>(getUserDataByName(duckVM, "duck-lisp"));
	dl_ptrdiff_t key = duckLisp_symbol_nameToValue(duckLisp, (dl_uint8_t *) globalName.c_str(), globalName.length());
	if (key < 0) {
		error("Global \"" + globalName + "\" is undefined.");
		return dl_error_invalidValue;
	}
	e = duckVM_pushGlobal(duckVM, key);
	if (e) return e;
	// stack: (object)-type

	{
		duckVM_object_type_t objectTypeType;
		e = duckVM_typeOf(duckVM, &objectTypeType);
		if (e) return e;
		if (objectTypeType != duckVM_object_type_type) {
			error("Global \"" + globalName + "\" should be a type value.");
			return dl_error_invalidValue;
		}
	}

	e = duckVM_copyType(duckVM, type);
	if (e) return e;

	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack:

	return e;
}

dl_error_t pushP2(duckVM_t *duckVM, dl_ptrdiff_t x, dl_ptrdiff_t y) {
	dl_error_t e = dl_error_ok;

	{
		dl_size_t p2Type;
		e = getGlobalType(duckVM, "p2-type", &p2Type);
		if (e) return e;

		e = duckVM_pushComposite(duckVM, p2Type);
		if (e) return e;
		// stack: (p2 nil)
	}

	e = duckVM_pushVector(duckVM, 2);
	if (e) return e;
	// stack: (p2 nil) [nil nil]
	e = duckVM_pushInteger(duckVM);
	if (e) return e;
	// stack: (p2 nil) [nil nil] 0
	e = duckVM_setInteger(duckVM, x);
	if (e) return e;
	// stack: (p2 nil) [nil nil] x
	e = duckVM_setElement(duckVM, 0, -2);
	if (e) return e;
	// stack: (p2 nil) [x nil] x
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: (p2 nil) [x nil]

	e = duckVM_pushInteger(duckVM);
	if (e) return e;
	// stack: (p2 nil) [x nil] 0
	e = duckVM_setInteger(duckVM, y);
	if (e) return e;
	// stack: (p2 nil) [x nil] y
	e = duckVM_setElement(duckVM, 1, -2);
	if (e) return e;
	// stack: (p2 nil) [x y] y
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: (p2 nil) [x y]
	e = duckVM_setCompositeValue(duckVM, -2);
	if (e) return e;
	// stack: (p2 [x y]) [x y]
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: (p2 [x y])

	return e;
}

dl_error_t copyP2(duckVM_t *duckVM, dl_ptrdiff_t *x, dl_ptrdiff_t *y, std::string widgetName, std::string fieldName) {
	dl_error_t e = dl_error_ok;
	std::string p2Error = widgetName + "::" + fieldName + " may only be assigned a P2.";
	std::string internalP2Error = "Internal representation of \"p2-type\" composites must be a vector of integers of length 2";

	// We expect the argument to be a vector of 2 integers wrapped in a composite of the type stored in the global
	// variable "p2-type".
	{
		duckVM_object_type_t objectType;
		e = duckVM_typeOf(duckVM, &objectType);
		if (e) return e;
		if (duckVM_object_type_composite != objectType) {
			error(p2Error);
			return dl_error_invalidValue;
		}
	}
	{
		dl_size_t compositeType;
		e = duckVM_copyCompositeType(duckVM, &compositeType);
		if (e) return e;

		dl_size_t p2Type;
		e = getGlobalType(duckVM, "p2-type", &p2Type);
		if (e) return e;

		if (compositeType != p2Type) {
			error(p2Error);
			return dl_error_invalidValue;
		}
	}
	e = duckVM_pushCompositeValue(duckVM);
	if (e) return e;
	// stack: value (composite-value value)

	// Now check if the value is a vector of length 3.
	{
		dl_bool_t isVector;
		e = duckVM_isVector(duckVM, &isVector);
		if (e) return e;
		if (!isVector) {
			error(internalP2Error);
			return dl_error_invalidValue;
		}
	}
	dl_size_t vector_length;
	e = duckVM_length(duckVM, &vector_length);
	if (e) return e;
	if (vector_length != 2) {
		error(internalP2Error);
		return dl_error_invalidValue;
	}

	// Check that every value is an integer and collect the integer values into a C++ vector.
	std::vector<int> vec2 = {0, 0};
	for (size_t vector_index = 0; vector_index < vector_length; vector_index++) {
		e = duckVM_pushElement(duckVM, vector_index);
		if (e) return e;
		// stack: value (composite-value value) (elt (composite-value) ,vector_index)
		{
			dl_bool_t isInteger;
			e = duckVM_isInteger(duckVM, &isInteger);
			if (e) return e;
			if (!isInteger) {
				error(internalP2Error);
				return dl_error_invalidValue;
			}
		}
		{
			dl_ptrdiff_t integer;
			e = duckVM_copySignedInteger(duckVM, &integer);
			if (e) return e;
			// Everything looks good so far! Copy into temporary C++ p2 buffer.
			vec2[vector_index] = integer;  // Cast: dl_ptrdiff_t -> int
		}
		e = duckVM_pop(duckVM);
		if (e) return e;
		// stack: value (composite-value value)
	}
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: value

	// All checks passed. Perform the assignment.
	*x = vec2[0];
	*y = vec2[1];

	return e;
}


// Set fields in `GuiWidgetWindow`. `name` is the field name. The value to set the field to is the top object in the
// `duckVM` stack. Do not pop that object off. Only copy.
dl_error_t GuiWidgetWindow::setMember(duckVM_t *duckVM, const std::string name) {
	dl_error_t e = dl_error_ok;
	// stack: value
	std::string colorError = "window::background-color may only be assigned a color.";
	std::string internalColorError = "Internal representation of \"color-type\" composites must be a vector of integers of length 3, where each integer is in the range [0, 255]";

	if ("background-color" == name) {
		// We expect the argument to be a vector of integers of length 3 wrapped in a composite of the type stored in
		// the global variable "color-type". Each integer must be in the range [0 255].
		{
			duckVM_object_type_t objectType;
			e = duckVM_typeOf(duckVM, &objectType);
			if (e) return e;
			if (duckVM_object_type_composite != objectType) {
				error(colorError);
				return dl_error_invalidValue;
			}
		}
		{
			dl_size_t compositeType;
			e = duckVM_copyCompositeType(duckVM, &compositeType);
			if (e) return e;

			dl_size_t colorType;
			e = getGlobalType(duckVM, "color-type", &colorType);
			if (e) return e;

			if (compositeType != colorType) {
				error(colorError);
				return dl_error_invalidValue;
			}
		}
		e = duckVM_pushCompositeValue(duckVM);
		if (e) return e;
		// stack: value (composite-value value)

		// Now check if the value is a vector of length 3.
		{
			dl_bool_t isVector;
			e = duckVM_isVector(duckVM, &isVector);
			if (e) return e;
			if (!isVector) {
				error(internalColorError);
				return dl_error_invalidValue;
			}
		}
		dl_size_t vector_length;
		e = duckVM_length(duckVM, &vector_length);
		if (e) return e;
		if (vector_length != 3) {
			error(internalColorError);
			return dl_error_invalidValue;
		}

		// Check that every value is an integer between 0 and 255, and collect the integer values into a C++ vector.
		std::vector<uint8_t> vec3 = {0, 0, 0};
		for (size_t vector_index = 0; vector_index < 3; vector_index++) {
			e = duckVM_pushElement(duckVM, vector_index);
			if (e) return e;
			// stack: value (composite-value value) (elt (composite-value) ,vector_index)
			{
				dl_bool_t isInteger;
				e = duckVM_isInteger(duckVM, &isInteger);
				if (e) return e;
				if (!isInteger) {
					error(internalColorError);
					return dl_error_invalidValue;
				}
			}
			{
				dl_size_t integer;
				e = duckVM_copyUnsignedInteger(duckVM, &integer);
				if (e) return e;
				if (integer > 255) {
					error(internalColorError);
					return dl_error_invalidValue;
				}
				// Everything looks good so far! Copy into temporary C++ color buffer.
				vec3[vector_index] = integer;  // Cast: dl_size_t -> uint8_t
			}
			e = duckVM_pop(duckVM);
			if (e) return e;
			// stack: value (composite-value value)
		}
		e = duckVM_pop(duckVM);
		if (e) return e;
		// stack: value

		// All checks passed. Perform the assignment.
		for (int i = 0; i < vector_length; i++) {
			backgroundColor[i] = vec3[i];
		}
	}
	else {
		error("\"" + name + "\" is not a valid GUI window widget field name.");
		return dl_error_invalidValue;
	}

	return e;
}

// Push a field in `GuiWidgetWindow` on the stack. `name` is the field name.
dl_error_t GuiWidgetWindow::pushMember(duckVM_t *duckVM, const std::string name) {
	dl_error_t e = dl_error_ok;
	// stack:

	if ("background-color" == name) {
		{
			dl_size_t colorType;
			e = getGlobalType(duckVM, "color-type", &colorType);
			if (e) return e;

			e = duckVM_pushComposite(duckVM, colorType);
			if (e) return e;
			// stack: (color nil)
		}

		static constexpr size_t vector_length = 3;
		e = duckVM_pushVector(duckVM, vector_length);
		if (e) return e;
		// stack: (color nil) [nil nil nil]
		for (int i = 0; i < vector_length; i++) {
			e = duckVM_pushInteger(duckVM);
			if (e) return e;
			// stack: (color nil) […] 0
			e = duckVM_setInteger(duckVM, backgroundColor[i]);
			if (e) return e;
			// stack: (color nil) […] backgroundColor[i]
			e = duckVM_setElement(duckVM, i, -2);
			if (e) return e;
			e = duckVM_pop(duckVM);
			if (e) return e;
			// stack: (color nil) […]
		}
		// stack: (color nil) [backgroundColor[0] backgroundColor[1] backgroundColor[2]]
		e = duckVM_setCompositeValue(duckVM, -2);
		if (e) return e;
		// stack: (color [backgroundColor[0] backgroundColor[1] backgroundColor[2]])
		//        [backgroundColor[0] backgroundColor[1] backgroundColor[2]]
		e = duckVM_pop(duckVM);
		if (e) return e;
		// stack: (color [backgroundColor[0] backgroundColor[1] backgroundColor[2]])
	}
	else {
		error("\"" + name + "\" is not a valid GUI window widget field name.");
		return dl_error_invalidValue;
	}

	return e;
}

// Set fields in `GuiWidgetImage`. `name` is the field name. The value to set the field to is the top object in the
// `duckVM` stack. Do not pop that object off. Only copy.
dl_error_t GuiWidgetImage::setMember(duckVM_t *duckVM, const std::string name) {
	dl_error_t e = dl_error_ok;
	const char *widget = "image";
	// stack: value

	if ("file" == name) {
		// We expect the argument to be a string.
		{
			dl_bool_t isString;
			e = duckVM_isString(duckVM, &isString);
			if (e) return e;
			if (!isString) {
				error("image::file may only be assigned a file path (a string).");
				return dl_error_invalidValue;
			}
		}

		{
			dl_uint8_t *cFileString = nullptr;
			dl_size_t cFileString_length = 0;
			e = duckVM_copyString(duckVM, &cFileString, &cFileString_length);
			if (e) return e;
			file = assetNameToFileName(std::string((char *) cFileString, cFileString_length));
		}
	}
	else if ("position" == name) {
		dl_ptrdiff_t x, y;
		e = copyP2(duckVM, &x, &y, widget, name);
		if (e) return e;
		rect.x = x; rect.y = y;
	}
	else if ("dimensions" == name) {
		dl_ptrdiff_t w, h;
		e = copyP2(duckVM, &w, &h, widget, name);
		if (e) return e;
		rect.w = w; rect.h = h;
	}
	else {
		error("\"" + name + "\" is not a valid GUI image widget field name.");
		return dl_error_invalidValue;
	}

	return e;
}


// Push a field in `GuiWidgetWindow` on the stack. `name` is the field name.
dl_error_t GuiWidgetImage::pushMember(duckVM_t *duckVM, const std::string name) {
	dl_error_t e = dl_error_ok;
	// stack:

	if ("file" == name) {
		e = duckVM_pushString(duckVM, (dl_uint8_t *) file.c_str(), file.length());
		if (e) return e;
		// stack: file
	}
	else if ("position" == name) {
		e = pushP2(duckVM, rect.x, rect.y);
		if (e) return e;
	}
	else if ("dimensions" == name) {
		e = pushP2(duckVM, rect.w, rect.h);
		if (e) return e;
	}
	else {
		error("\"" + name + "\" is not a valid GUI image widget field name.");
		return dl_error_invalidValue;
	}

	return e;
}


dl_error_t gui_generator_present(duckLisp_t *c,
                                 duckLisp_compileState_t *cs,
                                 dl_array_t *a,
                                 duckLisp_ast_expression_t *ast) {
	// (funcall name)
	duckLisp_ast_identifier_t layoutName = ast->compoundExpressions[1].value.identifier;
	duckLisp_ast_compoundExpression_t ce;
	ce.type = duckLisp_ast_type_identifier;
	ce.value.identifier = layoutName;
	duckLisp_ast_expression_t ex;
	ex.compoundExpressions_length = 1;
	ex.compoundExpressions = &ce;
	return duckLisp_compile_expression(c, cs, a, layoutName.value, layoutName.value_length, &ex, nullptr);
}
