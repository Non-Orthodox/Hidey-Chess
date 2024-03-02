#include "scripting.hpp"
#include "duck-lisp/DuckLib/core.h"
#include "duck-lisp/duckVM.h"
#include "settings.h"
#include <iostream>

dl_error_t script_callback_print(duckVM_t *duckVM);


dl_error_t script_callback_printCons(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	/* stack: cons */
	dl_bool_t nil;
	duckVM_object_type_t type;

	e = duckVM_isNil(duckVM, &nil);
	if (e) goto cleanup;
	if (nil) {
		/* stack: () */
		printf("nil");
	}
	else {
		/* stack: (car . cdr) */
		e = duckVM_pushFirst(duckVM);
		if (e) goto cleanup;
		/* stack: (car . cdr) car */
		e = duckVM_isNil(duckVM, &nil);
		if (e) goto cleanup;
		e = duckVM_typeOf(duckVM, &type);
		if (e) goto cleanup;
		if (nil) {
			/* stack: (() . cdr) () */
			printf("nil");
		}
		else if (type == duckVM_object_type_cons) {
			/* stack: ((caar . cdar) . cdr) (caar . cdar) */
			printf("(");
			e = script_callback_printCons(duckVM);
			if (e) goto cleanup;
			/* stack: ((caar . cdar) . cdr) (caar . cdar) */
			printf(")");
		}
		else {
			/* stack: (car . cdr) car */
			e = script_callback_print(duckVM);
			if (e) goto cleanup;
			/* stack: (car . cdr) car */
		}
		/* stack: (car . cdr) car */
		e = duckVM_pop(duckVM);
		if (e) goto cleanup;
		/* stack: (car . cdr) */


		/* stack: (car . cdr) */
		e = duckVM_pushRest(duckVM);
		if (e) goto cleanup;
		/* stack: (car . cdr) cdr */
		e = duckVM_isNil(duckVM, &nil);
		if (e) goto cleanup;
		e = duckVM_typeOf(duckVM, &type);
		if (e) goto cleanup;
		if (nil || (type == duckVM_object_type_list)) {
			/* stack: (car . cdr) cdr */
			if (!nil) {
				/* stack: (car . (cadr . cddr)) (cadr . cddr) */
				printf(" ");
				e = script_callback_printCons(duckVM);
				if (e) goto cleanup;
				/* stack: (car . (cadr . cddr)) (cadr . cddr) */
			}
			/* stack: (car . cdr) cdr */
		}
		else {
			/* stack: (car . cdr) cdr */
			printf(" . ");
			e = script_callback_print(duckVM);
			if (e) goto cleanup;
			/* stack: (car . cdr) cdr */
		}
		/* stack: (car . cdr) cdr */
		e = duckVM_pop(duckVM);
		if (e) goto cleanup;
		/* stack: (car . cdr) */
	}
	/* stack: cons */

 cleanup: return e;
}

dl_error_t script_callback_print(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	/* stack: object */
	duckVM_object_type_t type;

	e = duckVM_typeOf(duckVM, &type);
	if (e) goto cleanup;

	switch (type) {
	case duckVM_object_type_symbol: {
		/* stack: symbol */
		dl_size_t id = 0;
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		e = duckVM_copySymbolName(duckVM, &string, &length);
		if (e) break;
		e = duckVM_copySymbolId(duckVM, &id);
		if (e) break;
		for (dl_size_t i = 0; i < length; i++) {
			putchar(string[i]);
		}
		e = DL_FREE(duckVM->memoryAllocation, &string);
		if (e) break;
		printf("→%lu", id);
	}
		break;
	case duckVM_object_type_string: {
		/* stack: string */
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		e = duckVM_copyString(duckVM, &string, &length);
		if (e) goto stringCleanup;
		for (dl_size_t i = 0; i < length; i++) {
			putchar(string[i]);
		}
		stringCleanup:
		e = DL_FREE(duckVM->memoryAllocation, &string);
		if (e) break;
	}
		break;
	case duckVM_object_type_integer: {
		/* stack: integer */
		dl_ptrdiff_t integer;
		e = duckVM_copySignedInteger(duckVM, &integer);
		if (e) break;
		printf("%li", integer);
		break;
	}
	case duckVM_object_type_float: {
		/* stack: float */
		double floatingPoint;
		e = duckVM_copyFloat(duckVM, &floatingPoint);
		if (e) break;
		printf("%f", floatingPoint);
		break;
	}
	case duckVM_object_type_bool: {
		/* stack: boolean */
		dl_bool_t boolean;
		e = duckVM_copyBoolean(duckVM, &boolean);
		if (e) break;
		printf("%s", boolean ? "true" : "false");
		break;
	}
	case duckVM_object_type_list: {
		/* stack: list */
		dl_bool_t nil;
		e = duckVM_isNil(duckVM, &nil);
		if (e) break;
		if (nil) {
			/* stack: () */
			printf("nil");
		}
		else {
			/* stack: (car cdr) */
			duckVM_object_type_t type;

			printf("(");

			e = duckVM_pushCar(duckVM);
			if (e) goto cleanup;
			/* stack: (car cdr) car */
			e = duckVM_isNil(duckVM, &nil);
			if (e) goto cleanup;
			e = duckVM_typeOf(duckVM, &type);
			if (e) goto cleanup;
			if (nil) {
				/* stack: (() cdr) () */
				printf("(nil)");
			}
			else if (type == duckVM_object_type_cons) {
				/* stack: ((caar cdar) cdr) (caar cdar) */
				printf("(");
				e = script_callback_printCons(duckVM);
				if (e) goto cleanup;
				/* stack: ((caar cdar) cdr) (caar cdar) */
				printf(")");
			}
			else {
				/* stack: (car cdr) car */
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				/* stack: (car cdr) car */
			}
			/* stack: (car cdr) car */
			e = duckVM_pop(duckVM);
			if (e) goto cleanup;
			/* stack: (car cdr) */

			e = duckVM_pushCdr(duckVM);
			if (e) goto cleanup;
			/* stack: (car cdr) cdr */
			e = duckVM_isNil(duckVM, &nil);
			if (e) goto cleanup;
			e = duckVM_typeOf(duckVM, &type);
			if (e) goto cleanup;
			if (nil) {
				/* stack: (car ()) () */
			}
			else if (type == duckVM_object_type_list) {
				/* stack: (car cdr) cdr */
				if (!nil) {
					/* stack: (car (cadr cddr)) (cadr cddr) */
					printf(" ");
					e = script_callback_printCons(duckVM);
					if (e) goto cleanup;
					/* stack: (car (cadr cddr)) (cadr cddr) */
				}
				/* stack: (car cdr) cdr */
			}
			else {
				/* stack: (car cdr) cdr */
				printf(" . ");
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				/* stack: (car cdr) cdr */
			}
			/* stack: (car cdr) cdr */
			e = duckVM_pop(duckVM);
			if (e) goto cleanup;
			/* stack: (car cdr) */

			printf(")");
		}
		/* stack: list */
	}
		break;
	case duckVM_object_type_closure: {
		/* stack: closure */
		dl_ptrdiff_t name;
		dl_uint8_t arity;
		dl_bool_t variadic;
		dl_size_t length;
		e = duckVM_copyClosureName(duckVM, &name);
		if (e) break;
		e = duckVM_copyClosureArity(duckVM, &arity);
		if (e) break;
		e = duckVM_copyClosureIsVariadic(duckVM, &variadic);
		if (e) break;
		e = duckVM_length(duckVM, &length);
		if (e) break;
		printf("(closure $%li #%u%s", name, arity, variadic ? "?" : "");
		if (e) break;
		DL_DOTIMES(k, length) {
			/* stack: closure */
			duckVM_object_type_t type;
			e = duckVM_typeOf(duckVM, &type);
			if (e) break;
			putchar(' ');
			if (type == duckVM_object_type_closure) {
				printf("#C");
			}
			else {
				e = duckVM_pushElement(duckVM, k);
				if (e) break;
				/* stack: closure closure[k] */
				e = script_callback_print(duckVM);
				if (e) break;
				/* stack: closure closure[k] */
				e = duckVM_pop(duckVM);
				if (e) break;
			}
			/* stack: closure */
		}
		printf(")");
	}
		break;
	case duckVM_object_type_vector: {
		/* stack: vector */
		dl_size_t length;
		dl_bool_t empty;
		e = duckVM_length(duckVM, &length);
		if (e) goto cleanup;
		printf("[");
		e = duckVM_isEmpty(duckVM, &empty);
		if (e) goto cleanup;
		if (!empty) {
			/* stack: vector */
			DL_DOTIMES(k, length) {
				/* stack: vector */
				if (k != 0) putchar(' ');
				e = duckVM_pushElement(duckVM, k);
				if (e) goto cleanup;
				/* stack: vector vector[k] */
				e = script_callback_print(duckVM);
				if (e) goto cleanup;
				/* stack: vector vector[k] */
				e = duckVM_pop(duckVM);
				if (e) goto cleanup;
				/* stack: vector */
			}
			/* stack: vector */
		}
		/* stack: vector */
		printf("]");
	}
		break;
	case duckVM_object_type_type: {
		/* stack: type */
		dl_size_t type;
		e = duckVM_copyType(duckVM, &type);
		printf("::%lu", type);
		break;
	}
	case duckVM_object_type_composite: {
		/* stack: composite */
		dl_size_t type;
		e = duckVM_copyCompositeType(duckVM, &type);
		if (e) goto cleanup;
		printf("(make-instance ::%lu ", type);
		e = duckVM_pushCompositeValue(duckVM);
		if (e) goto cleanup;
		/* stack: composite value */
		e = script_callback_print(duckVM);
		if (e) goto cleanup;
		/* stack: composite value */
		e = duckVM_pop(duckVM);
		if (e) goto cleanup;
		/* stack: composite */
		printf(" ");
		e = duckVM_pushCompositeFunction(duckVM);
		if (e) goto cleanup;
		/* stack: composite function */
		e = script_callback_print(duckVM);
		if (e) goto cleanup;
		/* stack: composite function */
		e = duckVM_pop(duckVM);
		if (e) goto cleanup;
		/* stack: composite */
		printf(")");
		break;
	}
	default: {
		/* stack: object */
		duckVM_object_type_t type;
		e = duckVM_typeOf(duckVM, &type);
		if (e) break;
		printf("print: Unsupported type. [%u]\n", type);
	}
	}
	if (e) goto cleanup;

 cleanup:
	fflush(stdout);
	return e;
}

// (setting-get <name>)
dl_error_t script_callback_get(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	// stack: object
	dl_bool_t isSymbol;
	e = duckVM_isSymbol(duckVM, &isSymbol);
	if (e) return e;
	if (!isSymbol) {
		e = dl_error_invalidValue;
		return e;
	}
	// stack: symbol
	dl_uint8_t *symbolString = nullptr;
	dl_size_t symbolString_length = 0;
	e = duckVM_copySymbolName(duckVM, &symbolString, &symbolString_length);
	if (e) return e;
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: EMPTY
	Setting* setting = g_settings->find(std::string((char *) symbolString, symbolString_length));
	settingsType_t settingType = setting->type;
	if (settingType == settingsType_boolean) {
		e = duckVM_pushBoolean(duckVM);
		if (e) return e;
		// stack: false
		e = duckVM_setBoolean(duckVM, setting->getBool());
		if (e) return e;
		// stack: setting->getBool()
	}
	else if (settingType == settingsType_integer) {
		e = duckVM_pushInteger(duckVM);
		if (e) return e;
		// stack: 0
		e = duckVM_setInteger(duckVM, setting->getInt());
		if (e) return e;
		// stack: setting->getInt()
	}
	else if (settingType == settingsType_string) {
		std::string string = setting->getString();
		e = duckVM_pushString(duckVM, (dl_uint8_t *) string.c_str(), string.length());
		if (e) return e;
		// stack: setting->getString()
	}
	else {
		// stack: EMPTY
		e = dl_error_invalidValue;
		return e;
	}
	return e;
}

// (setting-set <name> <value>)
dl_error_t script_callback_set(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	// stack: name value
	e = duckVM_push(duckVM, -2);
	if (e) return e;
	// stack: name value name
	dl_bool_t nameObjectIsSymbol;
	e = duckVM_isSymbol(duckVM, &nameObjectIsSymbol);
	if (e) return e;
	if (!nameObjectIsSymbol) {
		e = dl_error_invalidValue;
		return e;
	}
	dl_uint8_t *string = nullptr;
	dl_size_t length = 0;
	e = duckVM_copySymbolName(duckVM, &string, &length);
	if (e) return e;
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: name value
	Setting* setting = g_settings->find(std::string((char *) string, length));
	settingsType_t settingType = setting->type;
	duckVM_object_type_t valueObjectType;
	e = duckVM_typeOf(duckVM, &valueObjectType);
	if (e) return e;
	if ((settingType == settingsType_boolean) && (valueObjectType == duckVM_object_type_bool)) {
		dl_bool_t boolean;
		e = duckVM_copyBoolean(duckVM, &boolean);
		if (e) return e;
		setting->set((bool) boolean);
	}
	else if ((settingType == settingsType_integer) && (valueObjectType == duckVM_object_type_integer)) {
		dl_ptrdiff_t integer;
		e = duckVM_copySignedInteger(duckVM, &integer);
		if (e) return e;
		setting->set(static_cast<int>(integer));
	}
	else if ((settingType == settingsType_string) && (valueObjectType == duckVM_object_type_string)) {
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		e = duckVM_copyString(duckVM, &string, &length);
		if (e) return e;
		setting->set(std::string((char *) string, length));
	}
	else {
		e = dl_error_invalidValue;
		return e;
	}
	e = duckVM_copyFromTop(duckVM, -2);
	if (e) return e;
	// stack: value value
	e = duckVM_pop(duckVM);
	if (e) return e;
	// stack: value
	return e;
}

