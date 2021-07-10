#include "SDLevents.h"
#include <SDL2/SDL.h>
#include <iostream>
#include "types.h"

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

