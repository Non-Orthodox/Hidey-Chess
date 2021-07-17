#ifndef BOARD_H
#define BOARD_H

#include "types.h"
#include "piece.h"
#include <vector>
#include <iostream>

class tile
{
    public:
        int p = 0;
        int coords[2] = {0,0};
        int colorNum = 0;
        tile() {}
        tile(int colorNum)
        {
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
    public:
        int width, height;
        std::vector<piece> pieceList;
        std::vector<tile> tiles;

        board(int width, int height)
        {
            this->width = width;
            this->height = height;
            tiles.resize(width*height);
            for(int i = 0; i < (width*height); i++) 
            {
                tiles[i].setCoords(i%width,(i-(i%width))/width);
            }
            piece tempPiece(0,neutral);
            pieceList.push_back(tempPiece);
        }
        ~board() {}

        //note that killed pieces are kept in the pieceList at the moment
        void addPiece(int type, team_t team, int x, int y)
        {
            piece tempPiece(type,team);
            pieceList.push_back(tempPiece);
            tiles[(y*width)+x].p = pieceList.size()-1; //very uncertain of if this works
        }

        //if a piece is in the spot (x2,y2), then it must be moved or killed before moving another piece to (x2,y2)
        void movePiece(int x1, int y1, int x2, int y2)
        {
            if( (x1 != x2) || (y1 != y2) )
            {
                tiles[(y2*width)+x2].p = tiles[(y1*width)+x1].p;
                tiles[(y1*width)+x1].p = 0;
            }
        }

        void killPiece(int x, int y)
        {
            pieceList[tiles[(y*width)+x].p].kill();
        }

        int getTilePiece(int x, int y)
        {
            return(tiles[(y*width)+x].p);
        }

        void reset()
        {
            for(int i = 0; i < (width*height); i++) 
            {
                tiles[i].p = 0;
                pieceList.resize(1);
            }
        }
};

void standardChessBoardInit(board*);
void printChessBoard(board*);

#endif