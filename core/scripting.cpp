#include "scripting.hpp"
#include "duck-lisp/DuckLib/core.h"
#include "duck-lisp/duckVM.h"
#include "settings.h"
#include <iostream>

dl_error_t script_callback_print(duckVM_t *duckVM);


dl_error_t script_callback_printCons(duckVM_t *duckVM, duckVM_object_t *consObject) {
	dl_error_t e = dl_error_ok;

	if (consObject == nullptr) {
		printf("nil");
	}
	else {
		duckVM_cons_t cons = duckVM_object_getCons(*consObject);
		duckVM_object_t *carObject = cons.car;
		duckVM_object_t *cdrObject = cons.cdr;
		if (carObject == nullptr) {
			printf("nil");
		}
		else if (duckVM_typeOf(*carObject) == duckVM_object_type_cons) {
			printf("(");
			e = script_callback_printCons(duckVM, carObject);
			if (e) goto cleanup;
			printf(")");
		}
		else {
			e = duckVM_push(duckVM, carObject);
			if (e) goto cleanup;
			e = script_callback_print(duckVM);
			if (e) goto cleanup;
			e = duckVM_pop(duckVM, nullptr);
			if (e) goto cleanup;
		}


		if ((cdrObject == nullptr) || (duckVM_typeOf(*cdrObject) == duckVM_object_type_cons)) {
			if (cdrObject != nullptr) {
				printf(" ");
				e = script_callback_printCons(duckVM, cdrObject);
				if (e) goto cleanup;
			}
		}
		else {
			printf(" . ");
			e = duckVM_push(duckVM, cdrObject);
			if (e) goto cleanup;
			e = script_callback_print(duckVM);
			if (e) goto cleanup;
			e = duckVM_pop(duckVM, nullptr);
			if (e) goto cleanup;
		}
	}

 cleanup:

	return e;
}

dl_error_t script_callback_print(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	duckVM_object_t object;

	// e = duckVM_getArg(duckVM, &string, 0);
	e = duckVM_pop(duckVM, &object);
	if (e) goto cleanup;

	switch (duckVM_typeOf(object)) {
	case duckVM_object_type_symbol: {
		dl_size_t id = 0;
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		e = duckVM_object_getSymbol(duckVM->memoryAllocation, &id, &string, &length, object);
		if (e) break;
		for (dl_size_t i = 0; i < length; i++) {
			putchar(string[i]);
		}
		printf("→%lu", id);
	}
		break;
	case duckVM_object_type_string: {
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		e = duckVM_object_getString(duckVM->memoryAllocation, &string, &length, object);
		if (e) goto stringCleanup;
		for (dl_size_t i = 0; i < length; i++) {
			putchar(string[i]);
		}
		stringCleanup:
		e = DL_FREE(duckVM->memoryAllocation, &string);
		if (e) break;
	}
		break;
	case duckVM_object_type_integer:
		printf("%li", duckVM_object_getInteger(object));
		break;
	case duckVM_object_type_float:
		printf("%f", duckVM_object_getFloat(object));
		break;
	case duckVM_object_type_bool:
		printf("%s", duckVM_object_getBoolean(object) ? "true" : "false");
		break;
	case duckVM_object_type_list: {
		duckVM_list_t list = duckVM_object_getList(object);
		if (list == nullptr) {
			printf("nil");
		}
		else {
			duckVM_cons_t cons;
			e = duckVM_list_getCons(list, &cons);
			if (e) break;
			duckVM_object_t *carObject = cons.car;
			duckVM_object_t *cdrObject = cons.cdr;
			printf("(");

			if (carObject == nullptr) {
				printf("(nil)");
			}
			else if (duckVM_typeOf(*carObject) == duckVM_object_type_cons) {
				printf("(");
				e = script_callback_printCons(duckVM, carObject);
				if (e) goto cleanup;
				printf(")");
			}
			else {
				e = duckVM_push(duckVM, carObject);
				if (e) goto cleanup;
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto cleanup;
			}

			if (cdrObject == nullptr) {
			}
			else if (duckVM_typeOf(*cdrObject) == duckVM_object_type_cons) {
				if (cdrObject != nullptr) {
					printf(" ");
					e = script_callback_printCons(duckVM, cdrObject);
					if (e) goto cleanup;
				}
			}
			else {
				printf(" . ");
				e = duckVM_push(duckVM, cdrObject);
				if (e) goto cleanup;
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto cleanup;
			}

			printf(")");
		}
	}
		break;
	case duckVM_object_type_closure: {
		duckVM_closure_t closure = duckVM_object_getClosure(object);
		duckVM_upvalueArray_t upvalueArray;
		e = duckVM_closure_getUpvalueArray(closure, &upvalueArray);
		if (e) break;
		printf("(closure $%li #%u%s", closure.name, closure.arity, closure.variadic ? "?" : "");
		DL_DOTIMES(k, upvalueArray.length) {
			duckVM_object_t *upvalueObject = upvalueArray.upvalues[k];
			putchar(' ');
			if (upvalueObject == nullptr) {
				putchar('$');
				continue;
			}
			duckVM_upvalue_t upvalue = duckVM_object_getUpvalue(*upvalueObject);
			duckVM_upvalue_type_t upvalueType = upvalue.type;
			if (upvalueType == duckVM_upvalue_type_stack_index) {
				duckVM_object_t object = DL_ARRAY_GETADDRESS(duckVM->stack,
				                                             duckVM_object_t,
				                                             upvalue.value.stack_index);
				e = duckVM_push(duckVM, &object);
				if (e) goto cleanup;
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto cleanup;
			}
			else if (upvalueType == duckVM_upvalue_type_heap_object) {
				e = duckVM_push(duckVM, upvalue.value.heap_object);
				if (e) goto cleanup;
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto cleanup;
			}
			else {
				while (upvalue.type == duckVM_upvalue_type_heap_upvalue) {
					upvalue = duckVM_object_getUpvalue(*upvalue.value.heap_upvalue);
				}
				if (upvalue.type == duckVM_upvalue_type_stack_index) {
					e = duckVM_push(duckVM,
					                &DL_ARRAY_GETADDRESS(duckVM->stack,
					                                     duckVM_object_t,
					                                     upvalue.value.stack_index));
					if (e) goto cleanup;
					e = script_callback_print(duckVM);
					if (e) goto cleanup;
					e = duckVM_pop(duckVM, nullptr);
					if (e) goto cleanup;
				}
				else if (upvalue.type == duckVM_upvalue_type_heap_object) {
					e = duckVM_push(duckVM, upvalue.value.heap_object);
					if (e) goto cleanup;
					e = script_callback_print(duckVM);
					if (e) goto cleanup;
					e = duckVM_pop(duckVM, nullptr);
					if (e) goto cleanup;
				}
				else {
					printf("Invalid upvalue type: %i", upvalue.type);
					e = dl_error_invalidValue;
					goto cleanup;
				}
			}
		}
		printf(")");
	}
		break;
	case duckVM_object_type_vector: {
		duckVM_vector_t vector = duckVM_object_getVector(object);
		duckVM_internalVector_t internalVector;
		e = duckVM_vector_getInternalVector(vector, &internalVector);
		if (e) break;
		printf("[");
		if (vector.internal_vector != nullptr) {
			for (dl_ptrdiff_t k = vector.offset;
			     (dl_size_t) k < internalVector.length;
			     k++) {
				duckVM_object_t *value = nullptr;
				e = duckVM_internalVector_getElement(internalVector, &value, k);
				if (e) break;
				if (k != vector.offset) putchar(' ');
				e = duckVM_push(duckVM, value);
				if (e) goto cleanup;
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				e = duckVM_pop(duckVM, nullptr);
				if (e) goto cleanup;
			}
		}
		printf("]");
	}
		break;
	case duckVM_object_type_type:
		printf("::%lu", duckVM_object_getType(object));
		break;
	case duckVM_object_type_composite: {
		duckVM_composite_t composite = duckVM_object_getComposite(object);
		duckVM_internalComposite_t internalComposite;
		e = duckVM_composite_getInternalComposite(composite, &internalComposite);
		if (e) break;
		printf("(make-instance ::%lu ", internalComposite.type);
		e = duckVM_push(duckVM, internalComposite.value);
		if (e) goto cleanup;
		e = script_callback_print(duckVM);
		if (e) goto cleanup;
		e = duckVM_pop(duckVM, nullptr);
		if (e) goto cleanup;
		printf(" ");
		e = duckVM_push(duckVM, internalComposite.function);
		if (e) goto cleanup;
		e = script_callback_print(duckVM);
		if (e) goto cleanup;
		e = duckVM_pop(duckVM, nullptr);
		if (e) goto cleanup;
		printf(")");
		break;
	}
	default:
		printf("print: Unsupported type. [%u]\n", duckVM_typeOf(object));
	}
	if (e) goto cleanup;

	e = duckVM_push(duckVM, &object);
	if (e) {
		goto cleanup;
	}

 cleanup:

	fflush(stdout);

	return e;
}

dl_error_t script_callback_get(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	duckVM_object_t object;

	e = duckVM_pop(duckVM, &object);
	if (e) return e;
	if (object.type != duckVM_object_type_symbol) {
		e = dl_error_invalidValue;
		return e;
	}
	dl_size_t symbolId = 0;
	dl_uint8_t *symbolString = nullptr;
	dl_size_t symbolString_length = 0;
	e = duckVM_object_getSymbol(duckVM->memoryAllocation, &symbolId, &symbolString, &symbolString_length, object);
	Setting* setting = g_settings->find(std::string((char *) symbolString, symbolString_length));
	settingsType_t settingType = setting->type;
	if (settingType == settingsType_boolean) {
		object = duckVM_object_makeBoolean(setting->getBool());
	}
	else if (settingType == settingsType_integer) {
		object = duckVM_object_makeInteger(setting->getInt());
	}
	else if (settingType == settingsType_string) {
		std::string string = setting->getString();
		duckVM_object_t internalStringObject = duckVM_object_makeInternalString((dl_uint8_t *) string.c_str(),
		                                                                        string.length());
		duckVM_object_t *internalStringObjectPointer = nullptr;
		e = duckVM_allocateHeapObject(duckVM, &internalStringObjectPointer, internalStringObject);
		if (e) return e;
		object = duckVM_object_makeString(internalStringObjectPointer, 0, string.length());
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

	duckVM_object_t object1;
	duckVM_object_t object2;

	e = duckVM_pop(duckVM, &object2);
	if (e) return e;
	e = duckVM_pop(duckVM, &object1);
	if (e) return e;
	if (object1.type != duckVM_object_type_symbol) {
		e = dl_error_invalidValue;
		return e;
	}
	dl_size_t id = 0;
	dl_uint8_t *string = nullptr;
	dl_size_t length = 0;
	e = duckVM_object_getSymbol(duckVM->memoryAllocation, &id, &string, &length, object1);
	if (e) return e;
	Setting* setting = g_settings->find(std::string((char *) string, length));
	settingsType_t settingType = setting->type;
	duckVM_object_type_t objectType = object2.type;
	if ((settingType == settingsType_boolean) && (objectType == duckVM_object_type_bool)) {
		setting->set((bool) duckVM_object_getBoolean(object2));
	}
	else if ((settingType == settingsType_integer) && (objectType == duckVM_object_type_integer)) {
		setting->set(static_cast<int>((int) duckVM_object_getInteger(object2)));
	}
	else if ((settingType == settingsType_string) && (objectType == duckVM_object_type_string)) {
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		e = duckVM_object_getString(duckVM->memoryAllocation, &string, &length, object2);
		if (e) return e;
		setting->set(std::string((char *) string, length));
	}
	else {
		e = dl_error_invalidValue;
		return e;
	}
	e = duckVM_push(duckVM, &object2);
	return e;
}

