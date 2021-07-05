#define SDL_MAIN_HANDLED

#include <iostream>
#include <SDL2/SDL.h>
#include "piece.h"

int main(){
    piece pawn(1,1,black);
    int* x = pawn.giveCoords();
    std::cout << x[0] << " " << x[1] << std::endl;
    pawn.setCoords(2,3);
    pawn.giveCoords();
    std::cout << x[0] << " " << x[1] << std::endl;
    return 0;
}