#ifndef BASIC_VISUALS_H
#define BASIC_VISUALS_H

#include <vector>
#include <SDL2/SDL.h>
#include "types.h"
#include "gui.h"

void renderBoard(SDL_Renderer*, int, int, int, color_t, color_t);
void renderBoard_button(std::vector<Button> boardButtons, const int width, const int height);
void renderGui(std::vector<Button> guiButtons);
//void renderPieces();

#endif
