#pragma once

#include "duck-lisp.hpp"

class Repl {
public:
	Repl();
	~Repl();
	int repl_nonblocking(std::shared_ptr<DuckLisp> duckLisp, std::shared_ptr<DuckVM> duckVM);
};
