#pragma once

#include "render_window.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include "duck-lisp.hpp"

struct GuiWidgetWindow {
	std::vector<uint8_t> backgroundColor;
	GuiWidgetWindow() {
		backgroundColor = {0, 0, 0};
	}
	dl_error_t setMember(duckVM_t *duckVM, const std::string name);
};

struct GuiWidgetImage {};

struct GuiWidgetText {};

struct GuiWidgetButton {};

struct GuiWidgetGroup {};

enum GuiObjectType {
	invalid,
	window,
};

struct GuiObject {
	// A struct, because apparently unions are ridiculously hard to use in C++.
	GuiWidgetWindow window;
	// GuiWidgetImage image;
	// GuiWidgetText text;
	// GuiWidgetButton button;
	// GuiWidgetGroup group;
	GuiObjectType type;
	bool visible = true;
	GuiObject(GuiObjectType type);
};

class Gui {
private:
public:
	size_t allocateObject(GuiObjectType type);
	void freeObject(size_t objectIndex);
	GuiObject findObject(size_t objectIndex);
	std::vector<size_t> freeList;  // Object indices that are free to use.
	std::vector<GuiObject> objectPool;
	Gui(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp);
	void render(RenderWindow *window);
};
