#include "duck-lisp.hpp"
#include "macros.hpp"

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
