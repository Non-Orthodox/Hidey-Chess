#include "scripting.hpp"
#include "file_utilities.hpp"
#include <iostream>
#include <cstring>
#include <regex>
#include <string>
extern "C" {
#include "duck-lisp/duckLisp.h"
#include "duck-lisp/parser.h"
#include "duck-lisp/duckVM.h"
}
#include "settings.h"
#include "log.h"
#include "defer.hpp"

dl_error_t script_callback_print(duckVM_t *duckVM);


/* Parser actions */


/* (include <script-name>::String)
   Do not give the path or file extension. */
dl_error_t script_action_include(duckLisp_t *dlc, duckLisp_ast_compoundExpression_t *ce) {
	dl_error_t e = dl_error_ok;
	dl_error_t eError = dl_error_ok;
	dl_array_t eString;
	defer(dl_array_quit(&eString));
	(void) dl_array_init(&eString, dlc->memoryAllocation, sizeof(char), dl_array_strategy_double);

	duckLisp_ast_expression_t *expression = &ce->value.expression;
	FILE *sourceFile = NULL;
	int tempInt = 0;
	std::string sourceCode;
	duckLisp_ast_compoundExpression_t ast;
	(void) duckLisp_ast_compoundExpression_init(&ast);

	/* Check arguments for call and type errors. */

	e = duckLisp_checkArgsAndReportError(dlc, *expression, 2, dl_false);
	if (e) return e;

	e = duckLisp_checkTypeAndReportError(dlc,
	                                     expression->compoundExpressions[0].value.identifier,
	                                     expression->compoundExpressions[1],
	                                     duckLisp_ast_type_identifier);
	if (e) return e;

	auto scriptName = std::string((char *) expression->compoundExpressions[1].value.identifier.value,
	                              expression->compoundExpressions[1].value.identifier.value_length);

	// Sanitize script name. Must only contain alphanumeric characters.

	std::regex nameRegex("[a-z0-9]+");
	auto valid = std::regex_match(scriptName, nameRegex);
	if (!valid) {
		error("Attempted to include script with an illegal name: \"" + scriptName + "\"");
		return dl_error_invalidValue;
	}

	auto fileName = scriptNameToFileName(scriptName);
	debug("Including file \"" + fileName + "\".");

	/* Fetch script. */

	sourceCode += "(";

	sourceFile = fopen(fileName.c_str(), "r");
	if (sourceFile == NULL) {
		error("Could not open file \"" + fileName + "\".\n");
		return dl_error_nullPointer;
	}
	while ((tempInt = fgetc(sourceFile)) != EOF) {
		sourceCode += tempInt & 0xFF;
	}
	if (fclose(sourceFile)) {
		warning("Failed to close file \"" + fileName + "\".");
	}

	sourceCode += ")";

	/* Parse script. */

	{
		dl_ptrdiff_t index = 0;
		e = duckLisp_parse_compoundExpression(dlc,
		                                      dl_false,
		                                      (dl_uint8_t *) fileName.c_str(),
		                                      fileName.length(),
		                                      (dl_uint8_t *) sourceCode.c_str(),
		                                      sourceCode.length(),
		                                      &ast,
		                                      &index,
		                                      dl_true);
		if (e) return e;
	}

	// Free the original expression.
	{
		duckLisp_ast_compoundExpression_t ce;
		ce.type = duckLisp_ast_type_expression;
		ce.value.expression = *expression;
		e = duckLisp_ast_compoundExpression_quit(dlc->memoryAllocation, &ce);
		if (e) return e;
	}

	// Replace the expression with `__noscope` and then the body of the just read file.

	e = DL_MALLOC(dlc->memoryAllocation,
	              &expression->compoundExpressions,
	              ast.value.expression.compoundExpressions_length + 1,
	              duckLisp_ast_compoundExpression_t);
	if (e) return e;
	expression->compoundExpressions_length = ast.value.expression.compoundExpressions_length + 1;

	{
		const std::string identifierString("__noscope");
		duckLisp_ast_identifier_t identifier;
		identifier.value_length = identifierString.length();
		e = DL_MALLOC(dlc->memoryAllocation, &identifier.value, identifier.value_length, char);
		if (e) return e;
		// Yes, this is dirty macro abuse.
		(void) std::memcpy(identifier.value, identifierString.c_str(), identifierString.length() * sizeof(char));
		expression->compoundExpressions[0].type = duckLisp_ast_type_identifier;
		expression->compoundExpressions[0].value.identifier = identifier;
	}

	(void) std::memcpy(&expression->compoundExpressions[1],
	                   ast.value.expression.compoundExpressions,
	                   (ast.value.expression.compoundExpressions_length
	                    * sizeof(duckLisp_ast_compoundExpression_t)));

	(void) DL_FREE(dlc->memoryAllocation, &ast.value.expression.compoundExpressions);

	return e;
}



/* Callbacks */


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

// print value::Any
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

// (setting-get <name>::Symbol)::(Bool Integer String)
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

// (setting-set <name>::Symbol <value>::(Bool Integer String))
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
	Setting* setting = nullptr;
	auto key = std::string((char *) string, length);
	if (g_settings->contains(key)) {
		setting = g_settings->find(key);
	}
	else {
		error("Can't set setting \"" + key + "\" since it doesn't exist.");
		return dl_error_invalidValue;
	}
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
