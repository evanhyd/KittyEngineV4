#pragma once
#include "bitboard.h"

class Zobrist
{
	U64 position_key;

public:

	U64 GetPositionKey();
	void HashPiece(int piece, int square);
	void HashEnpassant(int square);
	void HashCastle(int castle);
	void HashSideToMove();
	void Clear();
	void GenerateNewKey(Bitboard* bitboards, int enpassant_square, int castle, int side_to_move);


private:

	static U64 piece_keys[12][64];
	static U64 enpassant_keys[64];
	static U64 castle_keys[16];
	static U64 side_to_move_key;

public:
	static void InitZobristKeys();
};