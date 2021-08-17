#pragma once
#include "bitboard.h"

class Boardstate
{
public:

	Bitboard bitboard[12];
	Bitboard occupancies[3];
	int side_to_move;
	int castle;
	int enpassant_square;


	void Clear();

	Boardstate() = default;
	Boardstate(Bitboard* new_bitboards, Bitboard* new_occupancies, int new_side_to_move, int new_castle, int new_enpassant_square);
};

