#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL.h>
#include "SDLevents.h"
#include "piece.h"

int main(){
    //Initializing SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL failed to initialize" << SDL_GetError() << std::endl;
        return 1; //later on use an exception
    }
    
    //Creating Window
    SDL_Window* window = SDL_CreateWindow("Hidey-Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 500, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) 
    {
		std::cerr << "Error: Window could not be created | SDL_Error " << SDL_GetError() << std::endl;
        return 1;
    }

    //Creating Renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr) 
    {
		std::cerr << "Error: Renderer could not be created | SDL_Error " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Rect rect;

    rect.h = rect.w = rect.x = rect.y = 100;
    int error = SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    error = SDL_RenderFillRect(renderer, &rect);
    rect.x = rect.y = 300;
    error = SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);
    rect.x = rect.y = 400;
    error = SDL_RenderFillRect(renderer, &rect);
    SDL_RenderPresent(renderer);

    int run = 1;
    while(run)
    {
        run = SDL_eventHandle();
    }

    //Destroying and Quitting
	SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
	window = nullptr;
	SDL_Quit();

    return 0;
}