#pragma once

#include <string>
#include <memory>
#include <vector>
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
	dl_error_t print_errors();
	int registerGenerator();
	int registerCallback(const std::string name, const std::string typeString, dl_error_t (*callback)(duckVM_t *));
};

class DuckVM {
public:
	duckVM_t duckVM;
	dl_memoryAllocation_t memoryAllocation;
	DuckVM(const std::size_t, const std::size_t);
	~DuckVM();
	dl_error_t print_errors();
	int registerCallback(std::ptrdiff_t index, dl_error_t (*callback)(duckVM_t *));
};

int registerCallback(std::shared_ptr<DuckVM> duckVM,
                     std::shared_ptr<DuckLisp> duckLisp,
                     const std::string name,
                     const std::string typeString,
                     dl_error_t (*callback)(duckVM_t *));
int eval(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp, const std::string source);
// int funcall(std::shared_ptr<DuckVM> duckVM,
//             std::shared_ptr<DuckLisp> duckLisp,
//             const std::string name,
//             const std::size_t numberOfArgs);
