#ifndef GUI_H
#define GUI_H

#include <stdexcept>
#include <iostream>
#include <SDL2/SDL.h>
#include "settings.h"
#include "types.h"

class Button {
private:
	bool currentlyPressed = false;
	Setting *setting;
	SDL_Rect rect;
	SDL_Renderer *renderer;
public:
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
	
	Button(Setting *setting, SDL_Renderer *renderer, SDL_Rect rect) {
		assert(setting->type == settingsType_boolean);
		this->setting = setting;
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
	bool check(SDL_MouseButtonEvent mouseButtonEvent) {
		int error = 0;
	
		bool mouseIsPressed = false;
		
		if ((mouseButtonEvent.x < rect.x) || (mouseButtonEvent.x > rect.x + rect.w) || (mouseButtonEvent.y < rect.y) || (mouseButtonEvent.y > rect.y + rect.h)) {
			if (toggle) {
				return toggleState;
			}
			return false;
		}
		
		for (int i = 0; i < 5; i++) {
			if (((1<<i) & buttonMask) && (mouseButtonEvent.button == i)) {
				mouseIsPressed = mouseButtonEvent.state;
			}
		}
		
		// First time mouse is pressed.
		if (!currentlyPressed && mouseIsPressed) {
			
			if (toggle) {
				if (!toggleOnUp) {
					toggleState = !toggleState;
					
					// // Toggle on press.
					// if (toggleState) {
					// }
					// else {
					// }
					setting->set(toggleState);
				}
			}
			else {
				toggleState = mouseIsPressed;
				setting->set(toggleState);
			}
		}
		// First time mouse is released.
		else if (currentlyPressed && !mouseIsPressed) {
			
			if (toggle) {
				if (toggleOnUp) {
					toggleState = !toggleState;
					
					// // Toggle on press.
					// if (toggleState) {
					// }
					// else {
					// }
					setting->set(toggleState);
				}
			}
			else {
				toggleState = mouseIsPressed;
				setting->set(toggleState);
			}
		}
		
		currentlyPressed = mouseIsPressed;
		
		return toggleState;
	}
};

#endif // GUI_H
