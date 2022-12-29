#include "duck-lisp.hpp"
#include <cstddef>
#include "duck-lisp/DuckLib/core.h"
#include "duck-lisp/duckLisp.h"
#include "duck-lisp/duckVM.h"
#include "macros.hpp"
#include "log.h"

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

	duckLisp.memoryAllocation = &memoryAllocation;

	e = duckLisp_init(&duckLisp);
	if (e) {
		FREE(duckLisp.memoryAllocation->memory);
		return;
	}
}

DuckLisp::~DuckLisp() {
	duckLisp_quit(&duckLisp);
	FREE(duckLisp.memoryAllocation->memory);
}

int DuckLisp::registerCallback(std::ptrdiff_t *index, std::string name) {
	dl_ptrdiff_t dlIndex;
	dl_error_t e = duckLisp_linkCFunction(&duckLisp, &dlIndex, name.c_str(), name.size());
	if (index != nullptr) *index = dlIndex;
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

	duckVM.memoryAllocation = &memoryAllocation;

	e = duckVM_init(&duckVM, maxObjects);
	if (e) {
		FREE(duckVM.memoryAllocation->memory);
		return;
	}
}

DuckVM::~DuckVM() {
	duckVM_quit(&duckVM);
	FREE(duckVM.memoryAllocation->memory);
}

int DuckVM::registerCallback(const std::ptrdiff_t index, dl_error_t (*callback)(duckVM_t *)) {
	return duckVM_linkCFunction(&duckVM, index, callback);
}



int registerCallback(std::shared_ptr<DuckVM> duckVM,
                     std::shared_ptr<DuckLisp> duckLisp,
                     const std::string name,
                     dl_error_t (*callback)(duckVM_t *)) {
	int error = 0;
	std::ptrdiff_t index;
	error = duckLisp->registerCallback(&index, name);
	if (error) return error;
	error = duckVM->registerCallback(index, callback);
	return error;
}

dl_error_t print_errors(dl_array_t *errors){
	dl_error_t e = dl_error_ok;
	while (errors->elements_length > 0) {
		duckLisp_error_t error;  /* Compile errors */
		e = dl_array_popElement(errors, (void *) &error);
		if (e) break;

		error(std::string(error.message, error.message_length));

		if (error.index == -1) {
			continue;
		}
	}
	return e;
}

int eval(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp, const std::string source) {
	dl_error_t loadError;
	dl_error_t runtimeError;
	unsigned char *bytecode = nullptr;
	dl_size_t bytecode_length = 0;
	loadError = duckLisp_loadString(&duckLisp->duckLisp,
	                                &bytecode,
	                                &bytecode_length,
	                                source.c_str(),
	                                (dl_size_t) source.length());
	if (loadError) {
		error("Failed to compile string. (" + std::string(dl_errorString[loadError]) + ")");
		return print_errors(&duckLisp->duckLisp.errors);
	}
	if ((*g_settings)[settingEnum_disassemble]->getBool()) {
		std::cout << duckLisp_disassemble(duckLisp->duckLisp.memoryAllocation, bytecode, bytecode_length) << std::endl;
	}
	duckLisp_object_t *return_value = nullptr;
	runtimeError = duckVM_execute(&duckVM->duckVM, return_value, bytecode);
	if (runtimeError) {
		error("VM encountered a runtime error. (" + std::string(dl_errorString[runtimeError]) + ")");
		return print_errors(&duckVM->duckVM.errors);
	}
	return 0;
}
