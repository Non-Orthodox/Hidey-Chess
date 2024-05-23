#include "duck-lisp.hpp"
#include <cstddef>
#include <iostream>
#include <vector>
#include "duck-lisp/DuckLib/array.h"
#include "duck-lisp/DuckLib/core.h"
#include "duck-lisp/DuckLib/memory.h"
#include "duck-lisp/duckLisp.h"
#include "duck-lisp/duckVM.h"
#include "macros.hpp"
#include "log.h"
#include "defer.hpp"

DuckLisp::DuckLisp(const std::size_t hunk_size) {
	int e = 0;
	void *hunk = malloc(hunk_size);
	if (!hunk) {
		e = dl_error_outOfMemory;
		return;
	}

	e = dl_memory_init(&memoryAllocation, hunk, hunk_size, dl_memoryFit_best);
	if (e) {
		FREE(duckLisp.memoryAllocation->memory);
		return;
	}

	e = duckLisp_init(&duckLisp, &memoryAllocation, 10000, 10000);
	if (e) {
		FREE(duckLisp.memoryAllocation->memory);
		return;
	}

	duckLisp.userData = &sharedUserData;
}

DuckLisp::~DuckLisp() {
	duckLisp_quit(&duckLisp);
	FREE(duckLisp.memoryAllocation->memory);
}

dl_error_t DuckLisp::print_errors() {
	dl_error_t e = dl_error_ok;
	if (duckLisp.errors.elements_length == 0) return e;
	(void) error(std::string((char *) duckLisp.inferrerLog.elements, duckLisp.inferrerLog.elements_length));
	e = dl_array_clear(&duckLisp.inferrerLog);
	if (e) return e;
	(void) error(std::string((char *) duckLisp.errors.elements, duckLisp.errors.elements_length));
	e = dl_array_clear(&duckLisp.errors);
	return e;
}

dl_error_t DuckLisp::registerParserAction(const std::string name,
                                          dl_error_t (*callback)(duckLisp_t*, duckLisp_ast_compoundExpression_t*)) {
	return duckLisp_addParserAction(&duckLisp, callback, (dl_uint8_t *) name.c_str(), name.length());
}

int DuckLisp::registerCallback(const std::string name,
                               const std::string typeString,
                               dl_error_t (*callback)(duckVM_t *)) {
	dl_error_t e = duckLisp_linkCFunction(&duckLisp,
	                                      callback,
	                                      (dl_uint8_t *) name.c_str(),
	                                      name.length(),
	                                      (dl_uint8_t *) typeString.c_str(),
	                                      typeString.length());
	return e;
}

void *DuckLisp::getUserDataByName(const std::string name) {
	return (*static_cast<std::map<std::string,void *>*>(duckLisp.userData))[name];
}

void DuckLisp::setUserDataByName(const std::string name, void *value) {
	(*static_cast<std::map<std::string,void *>*>(duckLisp.userData))[name] = value;
}



DuckVM::DuckVM(const std::size_t hunk_size, const std::size_t maxObjects) {
	int e = 0;
	void *hunk = malloc(hunk_size);
	if (!hunk) {
		e = dl_error_outOfMemory;
		return;
	}

	e = dl_memory_init(&memoryAllocation, hunk, hunk_size, dl_memoryFit_best);
	if (e) {
		FREE(duckVM.memoryAllocation->memory);
		return;
	}

	e = duckVM_init(&duckVM, &memoryAllocation, maxObjects);
	if (e) {
		FREE(duckVM.memoryAllocation->memory);
		return;
	}

	duckVM.userData = &sharedUserData;
}

DuckVM::~DuckVM() {
	duckVM_quit(&duckVM);
	FREE(duckVM.memoryAllocation->memory);
}

dl_error_t DuckVM::print_errors() {
	(void) error(std::string((char *) duckVM.errors.elements, duckVM.errors.elements_length));
	return dl_array_clear(&duckVM.errors);
}

int DuckVM::registerCallback(std::ptrdiff_t index, dl_error_t (*callback)(duckVM_t *)) {
	return duckVM_linkCFunction(&duckVM, index, callback);
}

dl_error_t DuckVM::garbageCollect() {
	return duckVM_garbageCollect(&duckVM);
}

void *DuckVM::getUserDataByName(const std::string name) {
	return (*static_cast<std::map<std::string,void *>*>(duckVM.userData))[name];
}

void DuckVM::setUserDataByName(const std::string name, void *value) {
	(*static_cast<std::map<std::string,void *>*>(duckVM.userData))[name] = value;
}



int registerCallback(std::shared_ptr<DuckVM> duckVM,
                     std::shared_ptr<DuckLisp> duckLisp,
                     const std::string name,
                     const std::string typeString,
                     dl_error_t (*callback)(duckVM_t *)) {
	int error = 0;
	std::ptrdiff_t index;
	error = duckLisp->registerCallback(name, typeString, callback);
	if (error) return error;
	error = duckVM->registerCallback(duckLisp_symbol_nameToValue(&duckLisp->duckLisp,
	                                                             (dl_uint8_t *) name.c_str(),
	                                                             name.length()),
	                                 callback);
	return error;
}

int eval(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp, const std::string source) {
	dl_error_t loadError;
	dl_error_t runtimeError;
	unsigned char *bytecode = nullptr;
	dl_size_t bytecode_length = 0;
	loadError = duckLisp_loadString(&duckLisp->duckLisp,
	                                true,
	                                &bytecode,
	                                &bytecode_length,
	                                (dl_uint8_t *) source.c_str(),
	                                source.length(),
	                                DL_STR("Unknown"));
	if (loadError) {
		error("Failed to compile string. (" + std::string((char *) dl_errorString[loadError]) + ")");
		return duckLisp->print_errors();
	}
	if (bytecode == nullptr) return 0;
	if ((*g_settings)[settingEnum_disassemble]->getBool()) {
		dl_array_t disassembly;
		dl_error_t e = duckLisp_disassemble(&disassembly,
		                                    duckLisp->duckLisp.memoryAllocation,
		                                    bytecode,
		                                    bytecode_length);
		if (e) return e;
		defer(dl_array_quit(&disassembly));
		std::cout << std::string((char *) disassembly.elements, disassembly.elements_length) << std::endl;
	}
	runtimeError = duckVM_execute(&duckVM->duckVM, bytecode, bytecode_length);
	// stack: object?
	if (runtimeError) {
		error("VM encountered a runtime error. (" + std::string((char *) dl_errorString[runtimeError]) + ")");
		return duckVM->print_errors();
	}
	if (duckVM->duckVM.stack.elements_length > 0) {
		dl_error_t e = duckVM_pop(&duckVM->duckVM);
		if (e) return e;
	}
	// stack: empty
	return 0;
}


// int funcall(std::shared_ptr<DuckVM> duckVM,
//             std::shared_ptr<DuckLisp> duckLisp,
//             const std::string name,
//             const std::size_t numberOfArgs) {
// 	dl_error_t e = dl_error_ok;
// 	dl_error_t eError = dl_error_ok;
// 	dl_error_t runtimeError;

// 	// stack: EMPTY

// 	if (numberOfArgs >= 256) {
// 		error("A max of 256 arguments is allowed.");
// 		e = dl_error_invalidValue;
// 		return e;
// 	}

// 	std::ptrdiff_t key = duckLisp_symbol_nameToValue(&duckLisp->duckLisp, (dl_uint8_t *) name.c_str(), name.length());
// 	e = duckVM_pushGlobal(&duckVM->duckVM, key);
// 	if (e) {
// 		// error("Could not find global function \""
// 		//       + name
// 		//       + "\". ("
// 		//       + std::string((char *) dl_errorString[runtimeError])
// 		//       + ")");
// 		// eError = duckVM->print_errors();
// 		if (eError) e = eError;
// 		return e;
// 	}
// 	// stack: global-function

// 	// global sum lambda (a b c) print + a + b c
// 	duckVM_pushInteger(&duckVM->duckVM);
// 	duckVM_setInteger(&duckVM->duckVM, 5);
// 	duckVM_pushInteger(&duckVM->duckVM);
// 	duckVM_setInteger(&duckVM->duckVM, 7);
// 	duckVM_pushInteger(&duckVM->duckVM);
// 	duckVM_setInteger(&duckVM->duckVM, 2);
// 	runtimeError = duckVM_call(&duckVM->duckVM, -numberOfArgs-1, numberOfArgs);
// 	if (runtimeError) {
// 		error("VM encountered a runtime error. (" + std::string((char *) dl_errorString[runtimeError]) + ")");
// 		return duckVM->print_errors();
// 	}
// 	// stack: return-value

// 	return dl_error_ok;
// }

void *getUserDataByName(duckVM_t *duckVM, const std::string name) {
	return (*static_cast<std::map<std::string,void *>*>(duckVM->userData))[name];
}

void *getUserDataByName(duckLisp_t *duckLisp, const std::string name) {
	return (*static_cast<std::map<std::string,void *>*>(duckLisp->userData))[name];
}

void setUserDataByName(duckVM_t *duckVM, const std::string name, void *value) {
	(*static_cast<std::map<std::string,void *>*>(duckVM->userData))[name] = value;
}

void setUserDataByName(duckLisp_t *duckLisp, const std::string name, void *value) {
	(*static_cast<std::map<std::string,void *>*>(duckLisp->userData))[name] = value;
}
