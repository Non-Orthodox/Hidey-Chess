#include "controller.h"
#include "types.h"
#include <iostream>
#include <string>

void testController(board* board, gameState* GAME_STATE)
{
    std::string input;
    std::cout << "Move?" << std::endl;
    std::cin >> input;

    int coords[4];
    int index = 0;

    for(int i = 0; i < input.size(); i++)
    {
        if(std::isdigit(input[i]) && index < 4)
        {
            if( ((int)input[i] - (int)'0') < 8 )
            {
                coords[index] = (int)input[i] - (int)'0';
                index++;
            }
        }
    }
    if(index == 4)
    {
        board->movePiece(coords[0],coords[1],coords[2],coords[3]);
        std::cout << std::endl << std::endl;
        printChessBoard(board);
    }
    if(!input.compare(0,4,"stop"))
    {
        std::cout << "Now in main menu" << std::endl;
        *GAME_STATE = MAIN_MENU;
    }
}