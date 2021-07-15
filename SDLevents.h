#ifndef SDL_EVENTS_H
#define SDL_EVENTS_H

#include <vector>
#include <SDL2/SDL.h>
#include "types.h"
#include "gui.h"

int SDL_eventHandle(SDL_Event*,SDL_Window*,SDL_Renderer*);
void windowEventHandle(SDL_Event*,SDL_Window*,SDL_Renderer*);

//int MainMenuEventHandle();
//int UserTurnEventHandle();
//int EnemyTurnEventHandle();
int SP_EventHandle(SDL_Event* event,SDL_Window* window,SDL_Renderer* renderer,gameState* GAME_STATE,color_t p1Color,color_t p2Color, std::vector<Button> *boardButtons, int boardWidth, int boardHeight);
int MM_EventHandle( SDL_Event* event, SDL_Window* window, SDL_Renderer* renderer, gameState* GAME_STATE, color_t p1Color, color_t p2Color, std::vector<Button> *boardButtons, int boardWidth, int boardHeight);

#endif
