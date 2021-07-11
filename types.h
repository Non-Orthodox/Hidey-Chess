#ifndef TYPES_H
#define TYPES_H

struct teamColor 
{
    int red;
    int green;
    int blue;
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