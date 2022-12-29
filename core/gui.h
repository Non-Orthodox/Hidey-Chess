#ifndef GUI_H
#define GUI_H

#include <stdexcept>
#include <iostream>
#include <SDL2/SDL.h>
#include "types.h"

struct buttonState_t {
	bool value;
	bool transitioned;
};

class Button {
private:
	SDL_Rect rect;
	SDL_Renderer *renderer;
public:
	bool currentlyPressed = false;
	std::string name;
	// Greyed out and unpressable if false.
	bool active = true;
	// Toggle or momentary contact.
	bool toggle = false;
	// Toggle on mouse down or mouse up.
	bool toggleOnUp = false;
	bool toggleState = false;
	// Displayed or not.
	bool shown = true;
	color_t pressedColor = {0x7F, 0x7F, 0x7F};
	color_t releasedColor = {0xBF, 0xBF, 0xBF};
	color_t greyedColor = {0x3F, 0x3F, 0x3F};
	// Determines which buttons trigger an action.
	uint8_t buttonMask = (1<<SDL_BUTTON_LEFT);
	
	Button(std::string name, SDL_Renderer *renderer, SDL_Rect rect) {
		this->name = name;
		this->rect = rect;
		this->renderer = renderer;
	}
	
	~Button() {}
	
	void draw(void) {
		int error = 0;
	
		color_t color;
		
		if (!active) {
			color = greyedColor;
		}
		else if (toggleState) {
			color = pressedColor;
		}
		else {
			color = releasedColor;
		}
		
		error = SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue, SDL_ALPHA_OPAQUE);
		if (error) {
			throw std::logic_error("Button::draw");
		}
		error = SDL_RenderFillRect(renderer, &rect);
		if (error) {
			throw std::logic_error("Button::draw");
		}
	}
	
	/* Button::check
	returns:    State of button. In momentary contact mode, returns mouse
	;           button. In toggle mode, returns persistent toggle value. Mouse
	;           clicks will toggle the value.
	Side effects:
	;   Each button presse and release will run two callbacks.
	*/
	buttonState_t check(SDL_MouseButtonEvent mouseButtonEvent) {
		bool mouseIsPressed = false;
		bool transitioned;
		
		if ((mouseButtonEvent.x < rect.x) || (mouseButtonEvent.x > rect.x + rect.w) || (mouseButtonEvent.y < rect.y) || (mouseButtonEvent.y > rect.y + rect.h)) {
			// if (toggle) {
			// 	return toggleState;
			// }
			// return false;
			mouseIsPressed = toggleOnUp;
			transitioned = false;
		}
		else {
			for (int i = 0; i < 5; i++) {
				if (((1<<i) & buttonMask) && (mouseButtonEvent.button == i)) {
					mouseIsPressed = mouseButtonEvent.state;
				}
			}
			transitioned = mouseIsPressed ^ currentlyPressed;
		}
		
		// First time mouse is pressed.
		if (!currentlyPressed && mouseIsPressed) {
			
			if (toggle) {
				if (!toggleOnUp) {
					toggleState = !toggleState;
				}
			}
			else {
				toggleState = mouseIsPressed;
			}
		}
		// First time mouse is released.
		else if (currentlyPressed && !mouseIsPressed) {
			
			if (toggle) {
				if (toggleOnUp) {
					toggleState = !toggleState;
				}
			}
			else {
				toggleState = mouseIsPressed;
			}
		}
		
		currentlyPressed = mouseIsPressed;

		buttonState_t ret = {toggleState, transitioned};
		return ret;
	}
};

#endif // GUI_H
