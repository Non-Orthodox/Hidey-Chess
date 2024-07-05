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
	dl_error_t pushMember(duckVM_t *duckVM, const std::string name);
};

struct GuiWidgetImage {
	std::string file;
	SDL_Texture *texture = nullptr;
	SDL_Rect rect;

	dl_error_t setMember(duckVM_t *duckVM, const std::string name);
	dl_error_t pushMember(duckVM_t *duckVM, const std::string name);
};

struct GuiWidgetBoard {
	SDL_Rect rect;

	dl_error_t setMember(duckVM_t *duckVM, const std::string name);
	dl_error_t pushMember(duckVM_t *duckVM, const std::string name);
};

struct GuiWidgetText {};

struct GuiWidgetButton {};

enum GuiObjectType {
	GuiObjectType_free,
	GuiObjectType_invalid,
	GuiObjectType_window,
	GuiObjectType_image,
	GuiObjectType_board,
};

struct GuiObject {
	// A struct, because apparently unions are ridiculously hard to use in C++.
	GuiWidgetWindow window;
	GuiWidgetImage image;
	GuiWidgetBoard board;
	// GuiWidgetRectangle rectangle;
	// GuiWidgetText text;
	// GuiWidgetButton button;
	GuiObjectType type;
	bool visible = true;

	GuiObject(GuiObjectType type);
	dl_error_t pushVisible(duckVM_t *duckVM);
	dl_error_t setVisible(duckVM_t *duckVM);
};

class Gui {
private:
public:
	std::vector<size_t> freeList = {};  // Object indices that are free to use.
	std::vector<GuiObject> objectPool = {};

	int setupDucklisp(std::shared_ptr<DuckVM> duckVM, std::shared_ptr<DuckLisp> duckLisp);
	size_t allocateObject(GuiObjectType type);
	void freeObject(size_t objectIndex);
	GuiObject getObject(size_t objectIndex);
	void setObject(size_t objectIndex, GuiObject object);
	void render(RenderWindow *window);
};
