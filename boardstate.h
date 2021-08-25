#pragma once
#include "bitboard.h"

class Boardstate
{
public:

	Bitboard bitboards[12];
	Bitboard occupancies[3];
	int side_to_move;
	int castle;
	int enpassant_square;


	void Clear();

	Boardstate();
};

