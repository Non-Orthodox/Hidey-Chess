#pragma once

#include "render_window.hpp"
#include <memory>
#include "duck-lisp.hpp"

class Gui {
private:
	std::shared_ptr<DuckVM> duckVM;
	std::shared_ptr<DuckLisp> duckLisp;
public:
	void render(RenderWindow *window);
};
