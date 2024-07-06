#pragma once

extern "C" {
#include "duck-lisp/duckVM.h"
#include "duck-lisp/parser.h"
}

dl_error_t script_action_include(duckLisp_t *dlc, duckLisp_ast_compoundExpression_t *ce);
dl_error_t script_callback_print(duckVM_t *duckVM);
dl_error_t script_callback_get(duckVM_t *duckVM);
dl_error_t script_callback_set(duckVM_t *duckVM);
dl_error_t script_callback_gc(duckVM_t *duckVM);
