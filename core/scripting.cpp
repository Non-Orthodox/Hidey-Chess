#include "scripting.hpp"
#include "settings.h"
#include <iostream>

dl_error_t script_callback_print(duckVM_t *duckVM);


dl_error_t script_callback_printCons(duckVM_t *duckVM, duckLisp_object_t *cons) {
	dl_error_t e = dl_error_ok;

	if (cons == dl_null) {
		std::cout << "nil";
	}
	else {
		if ((cons->value.cons.car == dl_null)
		    || (cons->value.cons.car->type == duckLisp_object_type_cons)) {
			std::cout << "(";
			e = script_callback_printCons(duckVM, cons->value.cons.car);
			if (e) goto l_cleanup;
			std::cout << ")";
		}
		else {
			e = duckVM_push(duckVM, cons->value.cons.car);
			if (e) goto l_cleanup;
			e = script_callback_print(duckVM);
			if (e) goto l_cleanup;
			e = duckVM_pop(duckVM, nullptr);
			if (e) goto l_cleanup;
		}
		
		
		if ((cons->value.cons.cdr == dl_null)
		    || (cons->value.cons.cdr->type == duckLisp_object_type_cons)) {
			if (cons->value.cons.cdr != dl_null) {
				std::cout << " ";
				e = script_callback_printCons(duckVM, cons->value.cons.cdr);
				if (e) goto l_cleanup;
			}
		}
		else {
			std::cout << " . ";
			e = duckVM_push(duckVM, cons->value.cons.cdr);
			if (e) goto l_cleanup;
			e = script_callback_print(duckVM);
			if (e) goto l_cleanup;
			e = duckVM_pop(duckVM, nullptr);
			if (e) goto l_cleanup;
		}
	}
		
 l_cleanup:
	
	return e;
}

dl_error_t script_callback_print(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;
	
	duckLisp_object_t object;
	
	// e = duckVM_getArg(duckVM, &string, 0);
	e = duckVM_pop(duckVM, &object);
	if (e) goto l_cleanup;
	
	switch (object.type) {
	case duckLisp_object_type_symbol:
		for (dl_size_t i = 0; i < object.value.symbol.value_length; i++) {
			putchar(object.value.symbol.value[i]);
		}
		std::cout << "→" << object.value.symbol.id;
		break;
	case duckLisp_object_type_string:
		for (dl_size_t i = 0; i < object.value.string.value_length; i++) {
			putchar(object.value.string.value[i]);
		}
		break;
	case duckLisp_object_type_integer:
		std::cout << object.value.integer;
		break;
	case duckLisp_object_type_float:
		std::cout << object.value.floatingPoint;
		break;
	case duckLisp_object_type_bool:
		std::cout << (object.value.boolean ? "true" : "false");
		break;
	case duckLisp_object_type_list:
		if (object.value.list == dl_null) {
			std::cout << "nil";
		}
		else {
			std::cout << "(";

			if (object.value.list->value.cons.car == dl_null) {
				std::cout << "(nil)";
			}
			else if (object.value.list->value.cons.car->type == duckLisp_object_type_cons) {
				std::cout << "(";
				e = script_callback_printCons(duckVM, object.value.list->value.cons.car);
				if (e) goto l_cleanup;
				std::cout << ")";
			}
			else {
				e = duckVM_push(duckVM, object.value.list->value.cons.car);
				if (e) goto l_cleanup;
				e = script_callback_print(duckVM);
				if (e) goto l_cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto l_cleanup;
			}

			if (object.value.list->value.cons.cdr == dl_null) {
			}
			else if (object.value.list->value.cons.cdr->type == duckLisp_object_type_cons) {
				if (object.value.list->value.cons.cdr != dl_null) {
					std::cout << " ";
					e = script_callback_printCons(duckVM, object.value.list->value.cons.cdr);
					if (e) goto l_cleanup;
				}
			}
			else {
				std::cout << " . ";
				e = duckVM_push(duckVM, object.value.list->value.cons.cdr);
				if (e) goto l_cleanup;
				e = script_callback_print(duckVM);
				if (e) goto l_cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto l_cleanup;
			}

			std::cout << ")";
		}
		break;
	case duckLisp_object_type_closure:
		std::cout << "(closure " << object.value.closure.name;
		DL_DOTIMES(k, object.value.closure.upvalue_array->value.upvalue_array.length) {
			duckLisp_object_t *uv = object.value.closure.upvalue_array->value.upvalue_array.upvalues[k];
			putchar(' ');
			if (uv == dl_null) {
				putchar('$');
				continue;
			}
			if (uv->value.upvalue.type == duckVM_upvalue_type_stack_index) {
				duckLisp_object_t object = DL_ARRAY_GETADDRESS(duckVM->stack,
				                                               duckLisp_object_t,
				                                               uv->value.upvalue.value.stack_index);
				e = duckVM_push(duckVM, &object);
				if (e) goto l_cleanup;
				e = script_callback_print(duckVM);
				if (e) goto l_cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto l_cleanup;
			}
			else if (uv->value.upvalue.type == duckVM_upvalue_type_heap_object) {
				e = duckVM_push(duckVM, uv->value.upvalue.value.heap_object);
				if (e) goto l_cleanup;
				e = script_callback_print(duckVM);
				if (e) goto l_cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto l_cleanup;
			}
			else {
				while (uv->value.upvalue.type == duckVM_upvalue_type_heap_upvalue) {
					uv = uv->value.upvalue.value.heap_upvalue;
				}
				if (uv->value.upvalue.type == duckVM_upvalue_type_stack_index) {
					e = duckVM_push(duckVM,
					                &DL_ARRAY_GETADDRESS(duckVM->stack,
					                                     duckLisp_object_t,
					                                     uv->value.upvalue.value.stack_index));
					if (e) goto l_cleanup;
					e = script_callback_print(duckVM);
					if (e) goto l_cleanup;
					e = duckVM_pop(duckVM, nullptr);
					if (e) goto l_cleanup;
				}
				else if (uv->value.upvalue.type == duckVM_upvalue_type_heap_object) {
					e = duckVM_push(duckVM, uv->value.upvalue.value.heap_object);
					if (e) goto l_cleanup;
					e = script_callback_print(duckVM);
					if (e) goto l_cleanup;
					e = duckVM_pop(duckVM, nullptr);
					if (e) goto l_cleanup;
				}
			}
		}
		std::cout << ")";
		break;
	case duckLisp_object_type_vector:
		std::cout << "[";
		if (object.value.vector.internal_vector != dl_null) {
			for (dl_ptrdiff_t k = object.value.vector.offset;
			     (dl_size_t) k < object.value.vector.internal_vector->value.internal_vector.length;
			     k++) {
				duckLisp_object_t *value = object.value.vector.internal_vector->value.internal_vector.values[k];
				if (k != object.value.vector.offset) putchar(' ');
				e = duckVM_push(duckVM, value);
				if (e) goto l_cleanup;
				e = script_callback_print(duckVM);
				if (e) goto l_cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto l_cleanup;
			}
		}
		std::cout << "]";
		break;
	default:
		std::cout << "print: Unsupported type. [" << object.type << "]\n";
	}

	e = duckVM_push(duckVM, &object);
	if (e) {
		goto l_cleanup;
	}

	l_cleanup:

	fflush(stdout);

	return e;
}

dl_error_t script_callback_get(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	duckLisp_object_t object;

	e = duckVM_pop(duckVM, &object);
	if (e) return e;
	if (object.type != duckLisp_object_type_symbol) {
		e = dl_error_invalidValue;
		return e;
	}
	Setting* setting = g_settings->find(std::string(object.value.symbol.value, object.value.symbol.value_length));
	settingsType_t settingType = setting->type;
	if (settingType == settingsType_boolean) {
		object.type = duckLisp_object_type_bool;
		object.value.boolean = setting->getBool();
	}
	else if (settingType == settingsType_integer) {
		object.type = duckLisp_object_type_integer;
		object.value.integer = setting->getInt();
	}
	else if (settingType == settingsType_string) {
		object.type = duckLisp_object_type_string;
		std::string string = setting->getString();
		// DANGER
		object.value.string.value = (char *) string.c_str();
		object.value.string.value_length = string.length();
	}
	else {
		e = dl_error_invalidValue;
		return e;
	}
	e = duckVM_push(duckVM, &object);
	return e;
}

dl_error_t script_callback_set(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	duckLisp_object_t object1;
	duckLisp_object_t object2;

	e = duckVM_pop(duckVM, &object2);
	if (e) return e;
	e = duckVM_pop(duckVM, &object1);
	if (e) return e;
	if (object1.type != duckLisp_object_type_symbol) {
		e = dl_error_invalidValue;
		return e;
	}
	Setting* setting = g_settings->find(std::string(object1.value.symbol.value, object1.value.symbol.value_length));
	settingsType_t settingType = setting->type;
	duckLisp_object_type_t objectType = object2.type;
	if ((settingType == settingsType_boolean) && (objectType == duckLisp_object_type_bool)) {
		setting->set((bool) object2.value.boolean);
	}
	else if ((settingType == settingsType_integer) && (objectType == duckLisp_object_type_integer)) {
		setting->set(static_cast<int>(object2.value.integer));
	}
	else if ((settingType == settingsType_string) && (objectType == duckLisp_object_type_string)) {
		setting->set(std::string(object2.value.string.value, object2.value.string.value_length));
	}
	else {
		e = dl_error_invalidValue;
		return e;
	}
	e = duckVM_push(duckVM, &object2);
	return e;
}

