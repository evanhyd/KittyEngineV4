#include "boardstate.h"
#include "board.h"
#include <algorithm>

using namespace std;


void Boardstate::Clear()
{
	fill(begin(this->bitboard), end(this->bitboard), 0);
	fill(begin(this->occupancies), end(this->occupancies), 0);
	this->side_to_move = WHITE;
	this->castle = 0b1111;
	this->enpassant_square = INVALID_SQUARE;
}

Boardstate::Boardstate(Bitboard* new_bitboards, Bitboard* new_occupancies, int new_side_to_move, int new_castle, int new_enpassant_square) :
	side_to_move(new_side_to_move), castle(new_castle), enpassant_square(new_enpassant_square)
{
	copy(new_bitboards, new_bitboards + 12, this->bitboard);
	copy(new_occupancies, new_occupancies + 3, this->occupancies);
}