#include "boardstate.h"
#include "board.h"
#include <algorithm>
#include <random>

using namespace std;



void Boardstate::HashPiece(int piece, int square)
{
	this->position_key ^= Boardstate::piece_keys[piece][square];
}
void Boardstate::HashEnpassant(int square)
{
	this->position_key ^= Boardstate::enpassant_keys[square];
}
void Boardstate::HashCastle(int castle)
{
	this->position_key ^= Boardstate::castle_keys[castle];
}
void Boardstate::HashSideToMove()
{
	this->position_key ^= Boardstate::side_to_move_key;
}

void Boardstate::GenerateNewKey()
{
	this->position_key = 0;

	//piece square
	for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
	{
		for (Bitboard cpy_bitboard = this->bitboards[piece]; cpy_bitboard; cpy_bitboard.PopBit())
		{
			int square = cpy_bitboard.GetLeastSigBit();
			this->position_key ^= Boardstate::piece_keys[piece][square];
		}
	}

	//enpassant
	if (this->enpassant_square != INVALID_SQUARE) this->position_key ^= Boardstate::enpassant_keys[this->enpassant_square];

	//castle
	this->position_key ^= Boardstate::castle_keys[this->castle];

	//side to move
	if (side_to_move == BLACK) this->position_key ^= Boardstate::side_to_move_key;
}









void Boardstate::Clear()
{
	fill(begin(this->bitboards), end(this->bitboards), 0);
	fill(begin(this->occupancies), end(this->occupancies), 0);
	this->side_to_move = WHITE;
	this->castle = 0;
	this->enpassant_square = INVALID_SQUARE;
	this->fifty_moves = 0;
	this->position_key = 0;
}


Boardstate::Boardstate() : bitboards{}, occupancies{}, side_to_move(WHITE), castle(0), enpassant_square(INVALID_SQUARE), fifty_moves(0), position_key(0)
{
}




//static members
U64 Boardstate::piece_keys[12][64] = {};
U64 Boardstate::enpassant_keys[64] = {};
U64 Boardstate::castle_keys[16] = {};
U64 Boardstate::side_to_move_key = {};

void Boardstate::InitZobristKeys()
{
	random_device dev;
	mt19937_64 rng(dev());
	uniform_int_distribution<U64> distribution;

	for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
	{
		for (int square = 0; square < 64; ++square)
		{
			Boardstate::piece_keys[piece][square] = distribution(rng);
		}
	}

	for (int square = 0; square < 64; ++square)
	{
		Boardstate::enpassant_keys[square] = distribution(rng);
	}

	// WK | WQ | BK | BQ == 16
	for (int i = 0; i < 16; ++i)
	{
		Boardstate::castle_keys[i] = distribution(rng);
	}

	Boardstate::side_to_move_key = distribution(rng);
}