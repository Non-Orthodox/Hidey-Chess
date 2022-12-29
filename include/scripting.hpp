#pragma once

extern "C" {
#include "duck-lisp/duckVM.h"
}

dl_error_t script_callback_print(duckVM_t *duckVM);
dl_error_t script_callback_get(duckVM_t *duckVM);
dl_error_t script_callback_set(duckVM_t *duckVM);
