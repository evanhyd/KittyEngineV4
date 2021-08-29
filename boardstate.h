#pragma once
#include "bitboard.h"

class Boardstate
{
public:

	Bitboard bitboards[12];
	Bitboard occupancies[3];
	int enpassant_square;
	int castle;
	int side_to_move;
	int fifty_moves;
	U64 position_key;


	void HashPiece(int piece, int square);
	void HashEnpassant(int square);
	void HashCastle(int castle);
	void HashSideToMove();
	void Clear();
	void GenerateNewKey();

	Boardstate();


private:

	static U64 piece_keys[12][64];
	static U64 enpassant_keys[64];
	static U64 castle_keys[16];
	static U64 side_to_move_key;

public:
	static void InitZobristKeys();
};

