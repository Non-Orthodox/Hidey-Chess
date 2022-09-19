
#include "controller.h"
#include <cstddef>
#include <iostream>
#include <string>
#include "types.h"
#include "log.h"

void testController(board* board, gameState* GAME_STATE)
{
    std::string input;
    std::cout << "Move?" << std::endl;
    std::cin >> input;

    int coords[4];
    int index = 0;

    for(ptrdiff_t i = 0; (size_t) i < input.size(); i++)
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
        debug("Now in main menu.");
        *GAME_STATE = MAIN_MENU;
    }
}
