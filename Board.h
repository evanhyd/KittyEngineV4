#pragma once
#include "bitboard.h"
#include <string>

enum : int
{
	WHITE, BLACK, BOTH
};

enum : int
{
	WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
	BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING
};

enum : int
{
	WK = 0b0001, 
	WQ = 0b0010, 
	BK = 0b0100, 
	BQ = 0b1000 
};


char PieceToFen(int piece);
int FenToPiece(char FEN);


class Board
{
public:
	Bitboard bitboard[12];
	Bitboard occupancies[3];

	int side_to_move;
	int castle;
	int enpassant_square;


	void ParseFEN(const std::string& str);
	void PrintBoard();

	Board();
};
