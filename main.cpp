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
    if (window == NULL) 
    {
		std::cerr << "Error: Window could not be created | SDL_Error " << SDL_GetError() << std::endl;
        return 1;
    }

    int run = 1;

    while(run)
    {
        run = SDL_eventHandle();
    }

    //Destroying Window
	SDL_DestroyWindow(window);
	window = NULL;
	
	SDL_Quit();

    return 0;
}