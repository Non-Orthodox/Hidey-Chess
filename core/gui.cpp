#include "gui.hpp"
#include "duck-lisp.hpp"

Gui::Gui(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp) {
	// Slightly less global than global. Only this VM instance can see this object.
	duckLisp->setUserDataByName("gui", this);
	duckVM->setUserDataByName("gui", this);
}

void Gui::render(RenderWindow *window) {
	
}
