#include "gui.hpp"
#include "duck-lisp.hpp"


Gui::Gui(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp) {
	// Slightly less global than global. Only this VM instance can see this object.
	duckLisp->setUserDataByName("gui", this);
	duckVM->setUserDataByName("gui", this);
}

void Gui::render(RenderWindow *window) {
	
}


// Functions for use by duck-lisp.

// gui-get-member object::GuiObject name::String
// gui-set-member object::GuiObject name::String value::Any
// gui-make-instance type-name::String
