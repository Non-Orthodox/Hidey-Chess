#include "basicVisuals.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "types.h"
#include "settings.h"

//x and y are the center coordinates of the board. p1Col and p2Col are the two different tile colors
void renderBoard(SDL_Renderer* renderer, int x, int y, int tileSize, color_t p1Col, color_t p2Col)
{
	int error;
	SDL_Rect rect;
	bool p1Tile = false;
	rect.h = rect.w = tileSize;
	rect.x = x - 4*tileSize;
	rect.y = y - 4*tileSize;

	for(int i = 0; i < 8; i++) 
	{
		for(int j = 0; j < 8; j++)
		{
			//boolean flip
			p1Tile = !p1Tile;

			//choose color
			if(p1Tile)
				error = SDL_SetRenderDrawColor(renderer, p1Col.red, p1Col.green, p1Col.blue, SDL_ALPHA_OPAQUE);
			else
				error = SDL_SetRenderDrawColor(renderer, p2Col.red, p2Col.green, p2Col.blue, SDL_ALPHA_OPAQUE);

			//place rect
			error = SDL_RenderFillRect(renderer, &rect);

			//update one coordinate
			rect.x += tileSize;
		}
		//reset first coordinate
		rect.x = x - 4*tileSize;

		//update the other coordinate
		rect.y += tileSize;

		//boolean flip
		p1Tile = !p1Tile;
	}
}

//x and y are the center coordinates of the board. p1Col and p2Col are the two different tile colors
void renderBoard_button(std::vector<Button> boardButtons, const int width, const int height)
{
	for(int y = 0; y < height; y++) 
	{
		for(int x = 0; x < width; x++)
		{
			try {
				boardButtons[width * y + x].draw();
			}
			catch (std::logic_error& e) {
				std::cerr << "Error while drawing board." << std::endl;
			}
		}
	}
}
