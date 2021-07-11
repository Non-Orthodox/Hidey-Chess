#ifndef GUI_H
#define GUI_H

#include <stdexcept>
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
	bool toggleState = false;
	// Displayed or not.
	bool shown = true;
	color_t pressedColor = {0x7F, 0x7F, 0x7F};
	color_t releasedColor = {0xBF, 0xBF, 0xBF};
	color_t greyedColor = {0x3F, 0x3F, 0x3F};
	// Determines which buttons trigger an action.
	uint8_t buttonMask = (1<<SDL_BUTTON_LEFT);
	
	void callback_mousePressed(void);
	void callback_mouseReleased(void);
	void callback_buttonTransistionOn(void);
	void callback_buttonTransistionOff(void);
	
	Button(Setting *setting, SDL_Renderer *renderer, SDL_Rect rect) {
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
		else if (currentlyPressed) {
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
		
		for (int i = 0; i < 5; i++) {
			if (((1<<i) & buttonMask) && (mouseButtonEvent.button == i)) {
				mouseIsPressed = true;
			}
		}
		
		// First time mouse is pressed.
		if (!currentlyPressed && mouseIsPressed) {
			currentlyPressed = true;
			
			if (toggle) {
				toggleState = !toggleState;
				
				// Toggle on press.
				if (toggleState) {
					callback_buttonTransistionOn();
				}
				else {
					callback_buttonTransistionOff();
				}
			}
			else {
				toggleState = mouseIsPressed;
				
				callback_buttonTransistionOn();
			}
			
			// Run pressed callback.
			callback_mousePressed();
		}
		
		// First time mouse is released.
		if (currentlyPressed && !mouseIsPressed) {
			currentlyPressed = false;
			
			if (toggle) {
				toggleState = !toggleState;
				
				// No toggling on release... yet.
			}
			else {
				toggleState = mouseIsPressed;
				
				callback_buttonTransistionOff();
			}
			
			// Run released callback.
			callback_mouseReleased();
		}
		
		return toggleState;
	}
};

#endif // GUI_H
