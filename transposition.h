#pragma once
#include "move.h"
#include "bitboard.h"

class Transposition
{
public:
	U64 position_key;
	int depth;
	int score;
	int flag;
	Move best_move;

	Transposition(U64 new_position_key, int new_depth, int new_score, int new_flag, Move new_best_move = 0);


	static constexpr int HASH_FLAG_EXACT = 0;
	static constexpr int HASH_FLAG_ALPHA = 1;
	static constexpr int HASH_FLAG_BETA = 2;
};

