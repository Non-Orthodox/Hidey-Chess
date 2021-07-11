#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

struct color_t
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

enum pieceType {none=0,pawn,rook,knight,bishop,queen,king};

struct pieces
{
    pieceType type;
    bool white;
    bool highL;
};

enum gameState {NONE, MAIN_MENU, USER_TURN, ENEMY_TURN, SINGLEPLAYER};

#endif