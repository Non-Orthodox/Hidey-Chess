#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "piece.h"
#include <vector>

void standardChessBoardInit(piece [][8]);
void printChessBoard(piece [][8]);

class tile
{
    public:
        piece* p;
        int color;
        tile() {}
        tile(int color)
        {
            this->color = color;
        }
        tile(piece* piece, int color)
        {
            this->p = piece;
            this->color = color;
        }
        ~tile() {}
};

class board
{
    private:
        int width, height = 8;
    public:
        std::vector<piece> pieceList;
        tile* tiles;

        board() {}
        board(int width, int height)
        {

        }
        ~board() {}

};

#endif