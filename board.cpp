#include "board.h"
#include "types.h"
#include "piece.h"
#include <iostream>

void standardChessBoardInit(board* board)
{
	board->addPiece(2,white,0,0);
	board->addPiece(3,white,0,1);
	board->addPiece(4,white,0,2);
	board->addPiece(5,white,0,3);
	board->addPiece(6,white,0,4);
	board->addPiece(4,white,0,5);
	board->addPiece(3,white,0,6);
	board->addPiece(2,white,0,7);

	board->addPiece(2,black,7,0);
	board->addPiece(3,black,7,1);
	board->addPiece(4,black,7,2);
	board->addPiece(5,black,7,3);
	board->addPiece(6,black,7,4);
	board->addPiece(4,black,7,5);
	board->addPiece(3,black,7,6);
	board->addPiece(2,black,7,7);
}

void printChessBoard(board* board)
{
	piece* temp;
    std::cout << std::endl;
    for(int i = 0; i < 8; i++) 
    {
        for(int j = 0; j < 8; j++) 
        {
			temp = board->getTilePiece(j,i);
            if(temp->team == white)
                std::cout << "w" << temp->type << " ";
            else
                std::cout << "b" << temp->type << " ";
            
        }
        std::cout << std::endl << std::endl;
    }
}