#include "transposition.h"





void Transposition::Clear()
{
	this->position_key = 0;
	this->depth = 0;
	this->score = SCORE_EMPTY;
	this->hash_flag = HASH_FLAG_EMPTY;
	this->best_move.Clear();
}


Transposition::Transposition() : position_key(0), depth(0), score(SCORE_EMPTY), hash_flag(HASH_FLAG_EMPTY), best_move()
{
}

Transposition::Transposition(U64 new_position_key, int new_depth, int new_score, int new_hash_flag, Move new_best_move) : 
	position_key(new_position_key), depth(new_depth), score(new_score), hash_flag(new_hash_flag), best_move(new_best_move)
{
}


int Transposition::ProbeHash(const Transposition* tt, size_t tt_size, U64 position_key, int depth, int alpha, int beta)
{
	//works for power of 2
	size_t index = position_key & (tt_size - 1);
	const Transposition& entry = tt[index];

	if (entry.position_key == position_key && entry.depth <= depth)//liberal
	//if (entry.position_key == position_key && entry.depth == depth)//conservative
	{
		if (entry.hash_flag == HASH_FLAG_EXACT) return entry.score;
		else if (entry.hash_flag == HASH_FLAG_ALPHA && entry.score <= alpha) return alpha;
		else if (entry.hash_flag == HASH_FLAG_BETA && entry.score >= beta) return beta;
	}

	return SCORE_EMPTY;
}


void Transposition::RecordHash(Transposition* tt, size_t tt_size, const Transposition& t)
{
	size_t index = t.position_key & (tt_size - 1);
	if(t.depth <= tt[index].depth) tt[index] = t; //liberal
	//if (t.depth == tt[index].depth) tt[index] = t; //conservative
	//tt[index] = t; //tt
}