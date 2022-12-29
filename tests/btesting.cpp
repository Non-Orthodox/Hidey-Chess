#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main ()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        std::cout << "SDL_Init Error: " << SDL_GetError() << "\n";
    
    // if (!(IMG_Init(IMG_INIT_PNG)))
    //     std::cout << "IMG_Init Error: " << SDL_GetError() << "\n";



    return 0;
}