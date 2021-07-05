#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL.h>

int main(){
    int inp;
    std::cin >> inp;
    std::cout << "Number = " << inp << std::endl;
    return 0;
}