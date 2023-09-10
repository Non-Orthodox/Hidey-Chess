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

dl_error_t print_errors(dl_memoryAllocation_t *memoryAllocation, dl_array_t *errors){
	dl_error_t e = dl_error_ok;

	dl_array_t errorString;
	duckLisp_serialize_errors(memoryAllocation, &errorString, errors, NULL);
	error(std::string((char *) errorString.elements, errorString.elements_length));
	e = dl_array_quit(&errorString);
	if (e) goto cleanup;
 cleanup: return e;
}

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

	e = duckLisp_init(&duckLisp, &memoryAllocation, 10000);
	if (e) {
		FREE(duckLisp.memoryAllocation->memory);
		return;
	}
}

DuckLisp::~DuckLisp() {
	duckLisp_quit(&duckLisp);
	FREE(duckLisp.memoryAllocation->memory);
}

int DuckLisp::registerCallback(std::string name, dl_error_t (*callback)(duckVM_t *)) {
	dl_error_t e = duckLisp_linkCFunction(&duckLisp, callback, (dl_uint8_t *) name.c_str(), name.size());
	return e;
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
}

DuckVM::~DuckVM() {
	duckVM_quit(&duckVM);
	FREE(duckVM.memoryAllocation->memory);
}

int DuckVM::registerCallback(std::ptrdiff_t index, dl_error_t (*callback)(duckVM_t *)) {
	return duckVM_linkCFunction(&duckVM, index, callback);
}



int registerCallback(std::shared_ptr<DuckVM> duckVM,
                     std::shared_ptr<DuckLisp> duckLisp,
                     const std::string name,
                     dl_error_t (*callback)(duckVM_t *)) {
	int error = 0;
	std::ptrdiff_t index;
	error = duckLisp->registerCallback(name, callback);
	if (error) return error;
	error = duckVM->registerCallback(duckLisp_symbol_nameToValue(&duckLisp->duckLisp,
	                                                             (dl_uint8_t *) name.c_str(),
	                                                             name.size()),
	                                 callback);
	return error;
}

int eval(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp, const std::string source) {
	dl_error_t loadError;
	dl_error_t runtimeError;
	unsigned char *bytecode = nullptr;
	dl_size_t bytecode_length = 0;
	std::cout << source << std::endl;
	loadError = duckLisp_loadString(&duckLisp->duckLisp,
	                                &bytecode,
	                                &bytecode_length,
	                                (dl_uint8_t *) source.c_str(),
	                                source.length(),
	                                DL_STR("Unknown"));
	if (loadError) {
		error("Failed to compile string. (" + std::string((char *) dl_errorString[loadError]) + ")");
		return print_errors(&duckLisp->memoryAllocation, &duckLisp->duckLisp.errors);
	}
	if (bytecode == nullptr) return 0;
	if ((*g_settings)[settingEnum_disassemble]->getBool()) {
		dl_uint8_t *string = nullptr;
		dl_size_t length = 0;
		dl_error_t e = duckLisp_disassemble(&string,
		                                    &length,
		                                    duckLisp->duckLisp.memoryAllocation,
		                                    bytecode,
		                                    bytecode_length);
		if (e) return e;
		defer(DL_FREE(duckLisp->duckLisp.memoryAllocation, &string));
		std::cout << std::string((char *) string, length) << std::endl;
	}
	duckVM_object_t *return_value = nullptr;
	runtimeError = duckVM_execute(&duckVM->duckVM, return_value, bytecode, bytecode_length);
	if (runtimeError) {
		error("VM encountered a runtime error. (" + std::string((char *) dl_errorString[runtimeError]) + ")");
		return print_errors(&duckVM->memoryAllocation, &duckVM->duckVM.errors);
	}
	return 0;
}


int funcall(std::shared_ptr<DuckVM> duckVM,
            std::shared_ptr<DuckLisp> duckLisp,
            const std::string name,
            std::vector<duckVM_object_t> objects) {
	dl_error_t e = dl_error_ok;
	dl_error_t loadError;
	dl_error_t runtimeError;
	dl_size_t bytecode_length = 0;

	dl_uint8_t arity = objects.size();

	if (arity > 255) {
		error("Arguments vector must be less than 255 objects long.");
	}

	for (auto &object: objects) {
		e = duckVM_push(&duckVM->duckVM, &object);
		if (e) return e;
	}

	std::ptrdiff_t key = duckLisp_symbol_nameToValue(&duckLisp->duckLisp, (dl_uint8_t *) name.c_str(), name.size());
	duckVM_object_t global;
	e = duckVM_getGlobal(&duckVM->duckVM, &global, key);
	if (e) {
		error("Could not find global function \""
		      + name
		      + "\". ("
		      + std::string((char *) dl_errorString[runtimeError])
		      + ")");
		return print_errors(duckVM->duckVM.memoryAllocation, &duckVM->duckVM.errors);
	}

	// Push the value of the global on the stack.
	e = duckVM_push(&duckVM->duckVM, &global);
	if (e) return e;

	dl_uint8_t callerBytecode[] = {
		duckLisp_instruction_funcall8,
		1,
		arity
	};

	duckVM_object_t *return_value = nullptr;
	runtimeError = duckVM_execute(&duckVM->duckVM,
	                              return_value,
	                              callerBytecode,
	                              sizeof(callerBytecode)/sizeof(*callerBytecode));
	if (runtimeError) {
		error("VM encountered a runtime error. (" + std::string((char *) dl_errorString[runtimeError]) + ")");
		return print_errors(duckVM->duckVM.memoryAllocation, &duckVM->duckVM.errors);
	}
	return 0;
}
