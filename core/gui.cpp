#include "gui.hpp"
#include "duck-lisp.hpp"
#include "duck-lisp/duckVM.h"
#include "log.h"


dl_error_t gui_callback_makeInstance(duckVM_t *duckVM);
dl_error_t gui_callback_getMember(duckVM_t *duckVM);
dl_error_t gui_callback_setMember(duckVM_t *duckVM);


Gui::Gui(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp) {
	// Slightly less global than global. Only this VM instance can see this object.
	duckLisp->setUserDataByName("gui", this);
	duckVM->setUserDataByName("gui", this);
	registerCallback(duckVM, duckLisp, "gui-make-instance", "(I)", gui_callback_makeInstance);
	registerCallback(duckVM, duckLisp, "gui-get-member", "(I I)", gui_callback_getMember);
	registerCallback(duckVM, duckLisp, "gui-set-member", "(I I I)", gui_callback_setMember);
}

void Gui::render(RenderWindow *window) {
	// Call RenderWindow.
}



/* Functions for use by duck-lisp. */

// gui-make-instance type-name::String
dl_error_t gui_callback_makeInstance(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	debug("make-instance");

	duckVM_pop(duckVM);
	duckVM_pushNil(duckVM);

	return e;
}

// gui-get-member object::GuiObject name::String
dl_error_t gui_callback_getMember(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	debug("get-member");

	duckVM_pop(duckVM);
	duckVM_pop(duckVM);
	duckVM_pushNil(duckVM);

	return e;
}

// gui-set-member object::GuiObject name::String value::Any
dl_error_t gui_callback_setMember(duckVM_t *duckVM) {
	dl_error_t e = dl_error_ok;

	debug("set-member");

	duckVM_pop(duckVM);
	duckVM_pop(duckVM);
	duckVM_pop(duckVM);
	duckVM_pushNil(duckVM);

	return e;
}
