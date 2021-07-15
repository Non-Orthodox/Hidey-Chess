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
	return(this->coords);
}

void piece::move(int x, int y)
{
	coords[0] = x;
	coords[1] = y;
}