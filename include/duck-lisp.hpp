#pragma once

#include <string>
#include <memory>
extern "C" {
#include "duck-lisp/DuckLib/memory.h"
#include "duck-lisp/duckLisp.h"
}

class DuckLisp {
public:
	duckLisp_t duckLisp;
	dl_memoryAllocation_t memoryAllocation;
	DuckLisp(const std::size_t);
	~DuckLisp();
	int registerGenerator();
	int registerCallback(std::ptrdiff_t *index, std::string name);
};

class DuckVM {
public:
	duckVM_t duckVM;
	dl_memoryAllocation_t memoryAllocation;
	DuckVM(const std::size_t, const std::size_t);
	~DuckVM();
	int registerCallback(const std::ptrdiff_t index, dl_error_t (*callback)(duckVM_t *));
	int eval(const std::string);
};

int registerCallback(std::shared_ptr<DuckVM> duckVM,
                     std::shared_ptr<DuckLisp> duckLisp,
                     const std::string name,
                     dl_error_t (*callback)(duckVM_t *));
int eval(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp, const std::string);
int funcall(std::shared_ptr<DuckVM> duckVM, duckLisp_object_t *closure);
