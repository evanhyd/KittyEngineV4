#include "boardstate.h"
#include "board.h"
#include <algorithm>

using namespace std;


void Boardstate::Clear()
{
	fill(begin(this->bitboards), end(this->bitboards), 0);
	fill(begin(this->occupancies), end(this->occupancies), 0);
	this->side_to_move = WHITE;
	this->castle = 0;
	this->enpassant_square = INVALID_SQUARE;
}


Boardstate::Boardstate() : side_to_move(WHITE), castle(0), enpassant_square(INVALID_SQUARE)
{
	fill(begin(this->bitboards), end(this->bitboards), 0);
	fill(begin(this->occupancies), end(this->occupancies), 0);
}