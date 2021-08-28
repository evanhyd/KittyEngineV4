#include "transposition.h"
#include "board.h"




void Transposition::Clear()
{
	this->position_key = 0;
	this->depth_searched_beyond = 0;
	this->score = SCORE_EMPTY;
	this->hash_flag = HASH_FLAG_EMPTY;
}


Transposition::Transposition() : position_key(0), depth_searched_beyond(0), score(SCORE_EMPTY), hash_flag(HASH_FLAG_EMPTY)
{
}

Transposition::Transposition(U64 new_position_key, int new_depth_searched_beyond, int new_score, int new_hash_flag) : 
	position_key(new_position_key), depth_searched_beyond(new_depth_searched_beyond), score(new_score), hash_flag(new_hash_flag)
{
}


int Transposition::ProbeHashScore(const Transposition* tt, size_t tt_size, U64 position_key, int depth_searched_beyond, int alpha, int beta, int side_to_move)
{
	//works for power of 2
	size_t index = position_key & (tt_size - 1);
	const Transposition& entry = tt[index];

	if (entry.position_key == position_key && entry.depth_searched_beyond >= depth_searched_beyond)
	{
		int cached_score = (side_to_move == WHITE ? entry.score : -entry.score);

		if (entry.hash_flag == HASH_FLAG_EXACT) return cached_score;
		else if (entry.hash_flag == HASH_FLAG_ALPHA && cached_score <= alpha) return alpha;
		else if (entry.hash_flag == HASH_FLAG_BETA && cached_score >= beta) return beta;
	}

	return SCORE_EMPTY;
}

void Transposition::RecordHash(Transposition* tt, size_t tt_size, const Transposition& t, int side_to_move)
{
	size_t index = t.position_key & (tt_size - 1);
	if (t.depth_searched_beyond >= tt[index].depth_searched_beyond)
	{
		tt[index] = t;
		if (side_to_move == BLACK) tt[index].score = -tt[index].score;
	}
}