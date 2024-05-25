#include "gui.hpp"
#include "duck-lisp.hpp"
#include "duck-lisp/duckVM.h"
#include "log.h"
#include <cstddef>


dl_error_t gui_callback_makeInstance(duckVM_t *duckVM);
dl_error_t gui_callback_getMember(duckVM_t *duckVM);
dl_error_t gui_callback_setMember(duckVM_t *duckVM);


Gui::Gui(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp) {
	// Slightly less global than global. Only this VM instance can see this object.
	duckLisp->setUserDataByName("gui", this);
	duckVM->setUserDataByName("gui", this);
	registerCallback(duckVM, duckLisp, "gui-make-instance", "(I)", gui_callback_makeInstance);
	registerCallback(duckVM, duckLisp, "gui-get-member", "(I I)", gui_callback_getMember);
	registerCallback(duckVM, duckLisp, "gui-set-member", "(I I I)", gui_callback_setMember);
}

void Gui::render(RenderWindow *window) {
	// Call RenderWindow.
}


GuiObject::GuiObject(GuiObjectType type) {
	this->type = type;
	if (type == GuiObjectType::window) {
		this->window = GuiWidgetWindow();
	}
}

size_t allocateObject(Gui *gui, GuiObjectType type) {
	if (gui->freeList.empty()) {
		gui->objectPool.push_back(GuiObject(type));
		return gui->objectPool.size() - 1;
	}
	else {
		size_t index = gui->freeList.back();
		gui->freeList.pop_back();
		return index;
	}
}

void freeObject(Gui *gui, size_t objectIndex) {
	gui->freeList.push_back(objectIndex);
}

GuiObjectType getTypeFromName(const std::string typeName) {
	// Indexing type by name instead of integer is harder on C++, but this way the enumeration can't get out of sync
	// with DL.
	// If-else because I don't want to deal with a map.
	if ("window" == typeName) {
		return window;
	}
	return invalid;
}



/* Functions for use by duck-lisp. */

// gui-make-instance type-name::String
dl_error_t gui_callback_makeInstance(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));

	debug("make-instance");

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

	size_t objectIndex = allocateObject(gui, getTypeFromName(std::string((char *) cTypeName, cTypeName_length)));

	// FIXME: This is not an integer. This should be a new user-defined type.
	e = duckVM_pushInteger(duckVM);
	if (e) return e;
	e = duckVM_setInteger(duckVM, objectIndex);
	if (e) return e;

	return e;
}

// gui-get-member object::GuiObject name::String
dl_error_t gui_callback_getMember(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));

	debug("get-member");

	duckVM_pop(duckVM);
	duckVM_pop(duckVM);
	duckVM_pushNil(duckVM);

	return e;
}

// gui-set-member object::GuiObject name::String value::Any
dl_error_t gui_callback_setMember(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	Gui *gui = static_cast<Gui *>(getUserDataByName(duckVM, "gui"));

	debug("set-member");

	duckVM_pop(duckVM);
	duckVM_pop(duckVM);
	duckVM_pop(duckVM);
	duckVM_pushNil(duckVM);

	return e;
}
