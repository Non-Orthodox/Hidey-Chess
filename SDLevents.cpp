#include "SDLevents.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "types.h"
#include "basicVisuals.h"

int SDL_eventHandle(SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer)
{
	switch(event->type)
	{
	case SDL_QUIT:
		return 0;

	//MOUSE ACTIONS    
	case SDL_MOUSEMOTION:
		std::cout << event->motion.x << " " << event->motion.y << std::endl;
		break;
	case SDL_MOUSEBUTTONDOWN:
		std::cout << "mouseclick" << std::endl;
		break;
	case SDL_MOUSEBUTTONUP:
		break;
	
	//WINDOW ACTIONS
	case SDL_WINDOWEVENT:
		windowEventHandle(event, window, renderer);
		break;

	//PRINT NOT YET ADDED ACTIONS
	default:
		std::cout << event->type << std::endl;
		break;
	}
	return 1;
}

void windowEventHandle(SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer)
{
	switch(event->window.event)
	{
	case SDL_WINDOWEVENT_RESIZED:
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		//in the future, will have function which re-renders everything here
		break;
	case SDL_WINDOWEVENT_LEAVE:
		//std::cout << "mouse left window" << std::endl;
		break;
	case SDL_WINDOWEVENT_ENTER:
		//std::cout << "mouse entered window" << std::endl;
		break;
	case SDL_WINDOWEVENT_FOCUS_LOST:
		//std::cout << "window lost keyboard focus" << std::endl;
		break;
	case SDL_WINDOWEVENT_FOCUS_GAINED:
		//std::cout << "window gained keyboard focus" << std::endl;
		break;
	case SDL_WINDOWEVENT_SHOWN:
		break;
	case SDL_WINDOWEVENT_HIDDEN:
		break;
	case SDL_WINDOWEVENT_EXPOSED:
		break;
	case SDL_WINDOWEVENT_MOVED:
		break;
	case SDL_WINDOWEVENT_MINIMIZED:
		break;
	case SDL_WINDOWEVENT_MAXIMIZED:
		break;
	case SDL_WINDOWEVENT_RESTORED:
		break;

	default:
		std::cout << "unhandled window event" << std::endl;
		break;
	}
}

int MM_EventHandle( SDL_Event* event,
                    SDL_Window* window,
                    SDL_Renderer* renderer,
                    gameState* GAME_STATE,
                    color_t p1Color,
                    color_t p2Color,
                    std::vector<Button> *boardButtons,
                    std::vector<Button> *guiButtons,
                    int boardWidth,
                    int boardHeight)
{
	buttonState_t check;
	
	while (SDL_PollEvent(event)) {
		switch(event->type)
		{
		case SDL_QUIT:
			return 1;
	
		//MOUSE ACTIONS    
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			for (size_t i = 0; i < guiButtons->size(); i++) {
				check = (*guiButtons)[i].check(event->button);
				if (check.value) {
					if ((*guiButtons)[i].setting->name == "singleplayer") {
						*GAME_STATE = SINGLEPLAYER;
						
						// (*guiButtons)[i].active = false;
						for (size_t j = 0; j < guiButtons->size(); j++) {
							if ((*guiButtons)[j].setting->name == "multiplayer") {
								(*guiButtons)[j].active = false;
							}
						}
						break;
					}
					else if ((*guiButtons)[i].setting->name == "multiplayer") {
						*GAME_STATE = MULTIPLAYER;
						
						// (*guiButtons)[i].active = false;
						for (size_t j = 0; j < guiButtons->size(); j++) {
							if ((*guiButtons)[j].setting->name == "singleplayer") {
								(*guiButtons)[j].active = false;
							}
						}
						break;
					}
				}
			}
			break;
		
		//WINDOW ACTIONS
		case SDL_WINDOWEVENT:
			switch(event->window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				int winWidth, winHeight;
				//SDL_GetWindowSize(window, &winWidth, &winHeight);
				//renderBoard(renderer, winWidth/2, winHeight/2, winHeight/12, p1Color, p2Color);
				//render pieces
				SDL_RenderPresent(renderer);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				//std::cout << "mouse left window" << std::endl;
				break;
			case SDL_WINDOWEVENT_ENTER:
				//std::cout << "mouse entered window" << std::endl;
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				//std::cout << "window lost keyboard focus" << std::endl;
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				//std::cout << "window gained keyboard focus" << std::endl;
				break;
			case SDL_WINDOWEVENT_SHOWN:
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				break;
			case SDL_WINDOWEVENT_MOVED:
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				break;
			case SDL_WINDOWEVENT_RESTORED:
				break;
	
			default:
				std::cout << "unhandled window event" << std::endl;
				break;
			break;
			}
	
		//PRINT NOT YET ADDED ACTION TYPES
		default:
			std::cout << event->type << " not accounted for in SP handler" << std::endl;
			break;
		}
	}
	return 0;
}

int MP_EventHandle( SDL_Event* event,
                    SDL_Window* window,
                    SDL_Renderer* renderer,
                    gameState* GAME_STATE,
                    color_t p1Color,
                    color_t p2Color,
                    std::vector<Button> *boardButtons,
                    int boardWidth,
                    int boardHeight)
{
	
	while (SDL_PollEvent(event)) {
		switch(event->type)
		{
		case SDL_QUIT:
			return 1;
	
		//MOUSE ACTIONS    
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			for (int i = 0; i < boardWidth * boardHeight; i++) {
				(*boardButtons)[i].check(event->button);
			}
			break;
		
		//WINDOW ACTIONS
		case SDL_WINDOWEVENT:
			switch(event->window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				int winWidth, winHeight;
				//SDL_GetWindowSize(window, &winWidth, &winHeight);
				//renderBoard(renderer, winWidth/2, winHeight/2, winHeight/12, p1Color, p2Color);
				//render pieces
				SDL_RenderPresent(renderer);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				//std::cout << "mouse left window" << std::endl;
				break;
			case SDL_WINDOWEVENT_ENTER:
				//std::cout << "mouse entered window" << std::endl;
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				//std::cout << "window lost keyboard focus" << std::endl;
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				//std::cout << "window gained keyboard focus" << std::endl;
				break;
			case SDL_WINDOWEVENT_SHOWN:
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				break;
			case SDL_WINDOWEVENT_MOVED:
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				break;
			case SDL_WINDOWEVENT_RESTORED:
				break;
	
			default:
				std::cout << "unhandled window event" << std::endl;
				break;
			break;
			}
	
		//PRINT NOT YET ADDED ACTION TYPES
		default:
			std::cout << event->type << " not accounted for in SP handler" << std::endl;
			break;
		}
	}
	return 0;
}

int SP_EventHandle( SDL_Event* event,
                    SDL_Window* window,
                    SDL_Renderer* renderer,
                    gameState* GAME_STATE,
                    color_t p1Color,
                    color_t p2Color,
                    std::vector<Button> *boardButtons,
                    int boardWidth,
                    int boardHeight)
{
	
	buttonState_t check;// = {false, false};
	static int selected1_index = -1;
	
	while (SDL_PollEvent(event)) {
		switch(event->type)
		{
		case SDL_QUIT:
			return 1;
	
		//MOUSE ACTIONS
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			for (int i = 0; i < boardWidth * boardHeight; i++) {
				check = (*boardButtons)[i].check(event->button);
				if (check.transitioned) {
					if ((selected1_index == -1) && event->button.state) {
						for (int j = 0; j < boardWidth * boardHeight; j++) {
							if (i == j) {
								continue;
							}
							(*boardButtons)[j].toggle = false;
						}
						selected1_index = i;
					}
					else if ((selected1_index != i) && !event->button.state) {
						for (int j = 0; j < boardWidth * boardHeight; j++) {
							(*boardButtons)[j].toggle = true;
							(*boardButtons)[j].toggleState = false;
						}
						selected1_index = -1;
					}
				}
			}
			break;
		
		//WINDOW ACTIONS
		case SDL_WINDOWEVENT:
			switch(event->window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				int winWidth, winHeight;
				//SDL_GetWindowSize(window, &winWidth, &winHeight);
				//renderBoard(renderer, winWidth/2, winHeight/2, winHeight/12, p1Color, p2Color);
				//render pieces
				SDL_RenderPresent(renderer);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				//std::cout << "mouse left window" << std::endl;
				break;
			case SDL_WINDOWEVENT_ENTER:
				//std::cout << "mouse entered window" << std::endl;
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				//std::cout << "window lost keyboard focus" << std::endl;
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				//std::cout << "window gained keyboard focus" << std::endl;
				break;
			case SDL_WINDOWEVENT_SHOWN:
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				break;
			case SDL_WINDOWEVENT_MOVED:
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				break;
			case SDL_WINDOWEVENT_RESTORED:
				break;
	
			default:
				std::cout << "unhandled window event" << std::endl;
				break;
			break;
			}
	
		//PRINT NOT YET ADDED ACTION TYPES
		default:
			std::cout << event->type << " not accounted for in SP handler" << std::endl;
			break;
		}
	}
	return 0;
}
