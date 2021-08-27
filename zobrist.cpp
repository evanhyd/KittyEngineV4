#include "zobrist.h"
#include "board.h"
#include "boardstate.h"
#include <random>


U64 Zobrist::GetPositionKey()
{
	return this->position_key;
}

void Zobrist::HashPiece(int piece, int square)
{
	this->position_key ^= Zobrist::piece_keys[piece][square];
}
void Zobrist::HashEnpassant(int square)
{
	this->position_key ^= Zobrist::enpassant_keys[square];
}
void Zobrist::HashCastle(int castle)
{
	this->position_key ^= Zobrist::castle_keys[castle];
}
void Zobrist::HashSideToMove()
{
	this->position_key ^= Zobrist::side_to_move_key;
}


void Zobrist::Clear()
{
	this->position_key = 0;
}

void Zobrist::GenerateNewKey(Bitboard* bitboards, int enpassant_square, int castle, int side_to_move)
{
	this->position_key = 0;

	//piece square
	for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
	{
		for (Bitboard bitboard = bitboards[piece]; bitboard; bitboard.PopBit())
		{
			int square = bitboard.GetLeastSigBit();
			this->position_key ^= Zobrist::piece_keys[piece][square];
		}
	}

	//enpassant
	if (enpassant_square != INVALID_SQUARE) this->position_key ^= Zobrist::enpassant_keys[enpassant_square];

	//castle
	this->position_key ^= Zobrist::castle_keys[castle];
	
	//side to move
	if (side_to_move == BLACK) this->position_key ^= Zobrist::side_to_move_key;
}









//static members
U64 Zobrist::piece_keys[12][64] = {};
U64 Zobrist::enpassant_keys[64] = {};
U64 Zobrist::castle_keys[16] = {};
U64 Zobrist::side_to_move_key = {};

void Zobrist::InitZobristKeys()
{
	random_device dev;
	mt19937_64 rng(dev());
	uniform_int_distribution<U64> distribution;

	for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
	{
		for (int square = 0; square < 64; ++square)
		{
			Zobrist::piece_keys[piece][square] = distribution(rng);
		}
	}

	for (int square = 0; square < 64; ++square)
	{
		Zobrist::enpassant_keys[square] = distribution(rng);
	}

	// WK | WQ | BK | BQ == 16
	for (int i = 0; i < 16; ++i)
	{
		Zobrist::castle_keys[i] = distribution(rng);
	}

	Zobrist::side_to_move_key = distribution(rng);
}