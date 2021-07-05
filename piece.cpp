#include "piece.h"

piece::piece(int x, int y, team_t color)
{
    coords[0] = x;
    coords[1] = y;
    team = color;
}

piece::~piece () {}

int* piece::giveCoords()
{
    return(coords);
}

bool piece::isValidMove(int a, int b)
{
    return(true);
}

void piece::setCoords(int x, int y)
{
    coords[0] = x;
    coords[1] = y;
}