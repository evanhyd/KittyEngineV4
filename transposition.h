#pragma once
#include "move.h"
#include "bitboard.h"

class Transposition
{
public:
	U64 position_key;
	int depth;
	int score;
	int hash_flag;
	Move best_move;

	void Clear();

	Transposition();
	Transposition(U64 new_position_key, int new_depth, int new_score, int new_flag, Move new_best_move = 0);


	static constexpr int HASH_FLAG_EMPTY = 0;
	static constexpr int HASH_FLAG_EXACT = 1;
	static constexpr int HASH_FLAG_ALPHA = 2;
	static constexpr int HASH_FLAG_BETA = 3;
	static constexpr int SCORE_EMPTY = 1234567;

	//just ignore the collision lol
	static int ProbeHash(const Transposition* tt, size_t tt_size, U64 position_key, int depth, int alpha, int beta);
	static void RecordHash(Transposition* tt, size_t tt_size, const Transposition& t);
};

