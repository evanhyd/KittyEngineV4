#include "transposition.h"

Transposition::Transposition(U64 new_position_key, int new_depth, int new_score, int new_flag, Move new_best_move) : 
	position_key(new_position_key), depth(new_depth), score(new_score), flag(new_flag), best_move(new_best_move)
{
}