#include <SDL2/SDL.h>

struct teamColor 
{
    int red;
    int green;
    int blue;
};

void renderBoard(SDL_Renderer*, int, int, int, teamColor, teamColor);
