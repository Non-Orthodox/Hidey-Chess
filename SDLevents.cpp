#include "SDLevents.h"
#include <SDL2/SDL.h>
#include <iostream>

int SDL_eventHandle()
{
    SDL_Event event;

	while (SDL_PollEvent(&event)) 
    {
        switch(event.type)
        {
        case SDL_QUIT:
            return 0;

        //MOUSE ACTIONS    
        case SDL_MOUSEMOTION:
            std::cout << event.motion.x << " " << event.motion.y << std::endl;
            break;
        case SDL_MOUSEBUTTONDOWN:
            std::cout << "mouseclick" << std::endl;
            break;
        case SDL_MOUSEBUTTONUP:
            break;
        
        default:
            std::cout << event.type << std::endl;
            break;
        }
    }
    return 1;
}