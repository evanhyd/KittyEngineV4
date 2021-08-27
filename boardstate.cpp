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
	this->fifty_moves = 0;
	this->zobrist.Clear();
}


Boardstate::Boardstate() : side_to_move(WHITE), castle(0), enpassant_square(INVALID_SQUARE), fifty_moves(0)
{
	fill(begin(this->bitboards), end(this->bitboards), 0);
	fill(begin(this->occupancies), end(this->occupancies), 0);
	this->zobrist.Clear();
}