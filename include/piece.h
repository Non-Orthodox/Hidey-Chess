#ifndef PIECE_H
#define PIECE_H

typedef enum {neutral,white,black} team_t;
enum chessPieceType {none=0,pawn,rook,knight,bishop,queen,king};

class piece
{
	protected:
		int coords[2];
		bool alive = true;
	public:
		int type = 0;
		team_t team = neutral;

		piece() {}
		piece(int type, team_t team)
		{
			this->type = type;
			this->team = team;
		}

		~piece() {}

		int* giveCoords()
		{
			return(this->coords);
		}

		void move(int x, int y)
		{
			this->coords[0] = x;
			this->coords[1] = y;
		}

		void kill()
		{
			this->alive = false;
		}


};

#endif
