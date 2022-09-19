#pragma once

extern "C" {
#include "duck-lisp/DuckLib/memory.h"
#include "duck-lisp/duckLisp.h"
}

class DuckLisp {
	duckLisp_t duckLisp;
	dl_memoryAllocation_t memoryAllocation;
public:
	DuckLisp(const std::size_t hunk_size);
	~DuckLisp();
};
