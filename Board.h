#pragma once
#include "bitboard.h"
#include "fast_stack.h"
#include "move.h"
#include <string>
#include <vector>

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
	PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING
};

constexpr int PIECE_LIST_TABLE[2][6] =
{
	{WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING},
	{BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING}
};

enum : int
{
	WK = 0b0001, WQ = 0b0010, BK = 0b0100, BQ = 0b1000 
};

enum : int
{
	KING_SIDE, QUEEN_SIDE
};

constexpr int CASTLING_PERMISSION_FILTER_TABLE[64] = {
	 7, 15, 15, 15,  3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};

constexpr int CASTLE_PERMISSION_REQUIREMENT_TABLE[2][2] = { {WK, WQ}, {BK, BQ} }; //[side][castle_type]
constexpr U64 CASTLE_OCCUPANCY_MASK_TABLE[2][2] = { {6917529027641081856, 432345564227567616}, {96, 14} }; //[side][castle_type]


char PieceToFen(int piece);
int FenToPiece(char FEN);
int GetRank(int square);
int GetFile(int square);

class Board
{
public:

	Boardstate boardstate;
	FastStack boardstate_history;


	void ParseFEN(const std::string& str);

	bool IsSquareAttacked(int square, int attack_side);

	std::vector<Move> GenerateMoves(int side);
	bool MakeMove(Move move);

	void PerfTest(int depth, int& node_visited);

	void PrintBoard();

	Board(const string& FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
};

