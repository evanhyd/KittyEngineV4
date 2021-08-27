#pragma once
#include "bitboard.h"
#include "zobrist.h"

class Boardstate
{
public:

	Bitboard bitboards[12];
	Bitboard occupancies[3];
	int enpassant_square;
	int castle;
	int side_to_move;
	int fifty_moves;
	Zobrist zobrist;


	void Clear();

	Boardstate();
};

