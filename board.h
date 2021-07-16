#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "piece.h"
#include <vector>

class tile
{
    public:
        piece* p = nullptr;
        int coords[2] = {0,0};
        int colorNum = 0;
        tile() {}
        tile(int colorNum)
        {
            this->colorNum = colorNum;
        }
        tile(int colorNum, piece* piece)
        {
            this->p = piece;
            this->colorNum = colorNum;
        }
        ~tile() {}

        //This function and the existence of coords may be unnecessary, depending on controller structure
        void setCoords(int x, int y)
        {
            this->coords[0] = x;
            this->coords[1] = y;
        }
};

class board
{
    private:
        int width, height;
    public:
        std::vector<piece> pieceList;
        std::vector<tile> tiles;

        board(int width, int height)
        {
            tiles.resize(width*height);
            for(int i = 0; i < (width*height); i++) 
            {
                tiles[i].setCoords(i%width,(i-(i%width))/width);
            }
        }
        ~board() {}

        //note that killed pieces are kept in the pieceList at the moment
        void addPiece(int type, team_t team, int x, int y)
        {
            piece temp(type,team);
            pieceList.push_back(temp);
            piece* pointer = pieceList.data();
            tiles[(y*width)+x].p = pointer + pieceList.size() - 1; //very uncertain of if this works
        }

        //if a piece is in the spot (x2,y2), then it must be moved or killed before moving another piece to (x2,y2)
        void movePiece(int x1, int y1, int x2, int y2)
        {
            tiles[(y2*width)+x2].p = tiles[(y1*width)+x1].p;
            tiles[(y1*width)+x1].p = nullptr;
        }

        void killPiece(int x, int y)
        {
            (tiles[(y*width)+x].p)->kill();
        }

        piece* getTilePiece(int x, int y)
        {
            return(tiles[(y*width)+x].p);
        }

};

void standardChessBoardInit(board*);
void printChessBoard(board*);

#endif