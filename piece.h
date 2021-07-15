#ifndef PIECE_H
#define PIECE_H

typedef enum {neutral,white,black} team_t;
enum chessPieceType {none=0,pawn,rook,knight,bishop,queen,king};

class piece
{
	protected:
		int coords[2];
	public:
		int type = 0;
		team_t team = neutral;

		piece () {}

		piece (int x, int y, team_t team)
		{
			this->coords[0] = x;
			this->coords[1] = y;
			this->team = team;
		}

		~piece () {}

		int* giveCoords()
		{
			return(this->coords);
		}

		void move(int x, int y)
		{
			this->coords[0] = x;
			this->coords[1] = y;
		}
};


// class chessPiece : public piece 
// {
// 	public:
// 		bool highL = false;           //true if highlighted

// 		chessPiece () {}

// 		chessPiece (int x, int y, team_t team)
// 		{
// 			this->coords[0] = x;
// 			this->coords[1] = y;
// 			this->team = team;
// 		}

// 		~chessPiece () {}
// };

#endif
