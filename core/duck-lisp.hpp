#pragma once

#include <string>
extern "C" {
#include "duck-lisp/DuckLib/memory.h"
#include "duck-lisp/duckLisp.h"
}

class DuckLisp {
	duckLisp_t duckLisp;
	dl_memoryAllocation_t memoryAllocation;
public:
	DuckLisp(const std::size_t);
	~DuckLisp();
	int registerGenerator();
	int registerCallback();
	int eval(const std::string);
};

class DuckVM {
	duckVM_t duckVM;
	dl_memoryAllocation_t memoryAllocation;
public:
	DuckVM(const std::size_t, const std::size_t);
	~DuckVM();
	int addCallback();
	int eval(const std::string);
};
