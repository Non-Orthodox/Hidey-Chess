#include <cstdint>
#include <iostream>
#include <vector>
#include "settings.h"
#include "duck-lisp.hpp"

#define inline_vector(TYPE, VALUE) *(new std::vector<TYPE>(VALUE))
#define DlArgs(VALUE) inline_vector(duckLisp_object_t *, VALUE)

SettingsList *g_settings;

int main() {
	// DuckVM vm(100000, 1000);
	// uint8_t bytecode[100];
	// duckLisp_object_t closure;
	// vm.funcall(bytecode, &closure, DlArgs({new DLObject()}));
	return 0;
}
