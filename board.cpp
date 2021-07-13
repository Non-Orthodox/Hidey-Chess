#include "board.h"
#include "types.h"
#include <iostream>

void standardBoardInit(pieces board[][8])
{
    for(int i = 0; i < 8; i++) 
    {
        //setting piece types
        if((i == 0) || (i == 7)) 
        {
            board[i][0].type = board[i][7].type = rook;
            board[i][1].type = board[i][6].type = knight;
            board[i][2].type = board[i][5].type = bishop;
            board[i][3].type = king;
            board[i][4].type = queen;
        }
        else if((i == 1) || (i == 6))
        {
            for(int j = 0; j < 8; j++) 
            {
                board[i][j].type = pawn;
            }
        }
        else
        {
            for(int j = 0; j < 8; j++) 
            {
                board[i][j].type = none;
            }
        }

        //setting team
        if((i == 0)||(i == 1))
        {
            for(int j = 0; j < 8; j++) 
            {
                board[i][j].white = true;
                board[i][j].highL = false;
            }
        }
        else if((i == 6)||(i == 7))
        {
            for(int j = 0; j < 8; j++) 
            {
                board[i][j].white = false;
                board[i][j].highL = false;
            }
        }
    }
}

void printStandardBoard(pieces board[][8])
{
    std::cout << std::endl;
    for(int i = 0; i < 8; i++) 
    {
        for(int j = 0; j < 8; j++) 
        {
            if(board[j][i].white)
                std::cout << "w" << board[j][i].type << " ";
            else
                std::cout << "b" << board[j][i].type << " ";
            
        }
        std::cout << std::endl << std::endl;
    }
}