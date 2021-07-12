#ifndef SDL_EVENTS_H
#define SDL_EVENTS_H

#include <SDL2/SDL.h>
#include "types.h"

int SDL_eventHandle(SDL_Event*,SDL_Window*,SDL_Renderer*);
void windowEventHandle(SDL_Event*,SDL_Window*,SDL_Renderer*);

//int MainMenuEventHandle();
//int UserTurnEventHandle();
//int EnemyTurnEventHandle();
int SP_EventHandle(SDL_Event*,SDL_Window*,SDL_Renderer*,gameState*,teamColor,teamColor);

#endif