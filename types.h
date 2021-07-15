#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

struct color_t
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

enum gameState {NONE, MAIN_MENU, MULTIPLAYER, SINGLEPLAYER, CONSOLE_MODE};

#endif