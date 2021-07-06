typedef enum {black, white} team_t;

class piece
{
    private:
        team_t team;
        int coords[2];
        bool sameColor;               //if piece is on tile of same color
        bool highL = false;           //true if highlighted
    public:
        piece (int,int,team_t);       //constructor
        ~piece ();                    //destructor
        int* giveCoords();
        void move(int, int);
        bool isValidMove(int, int);  
};