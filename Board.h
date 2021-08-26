#pragma once
#include "bitboard.h"
#include "fast_stack.h"
#include "move.h"
#include <string>
#include <vector>
#include <deque>

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

enum : int
{
	WK = 0b0001, WQ = 0b0010, BK = 0b0100, BQ = 0b1000 
};

enum : int
{
	KING_SIDE, QUEEN_SIDE
};


char PieceToAscii(int piece);
char PieceToFen(int piece);
int FenToPiece(char FEN);
int FenToPieceType(char FEN);
int GetRank(int square);
int GetFile(int square);
int ToRank(char c);
int ToFile(char c);

class Board
{
	static constexpr int MAX_SEARCHING_DEPTH = 30;
	static constexpr int MAX_MOVE_PER_ROUND = 256;
	static constexpr int NULL_MOVE_DEPTH_REQUIRED = 1;
	static constexpr int NULL_MOVE_PIECE_REQUIRED = 2;
	static constexpr int LATE_MOVE_DEPTH_REQUIRED = 0;
	static constexpr int LATE_MOVE_SEARCHED_REQUIRED = 4;
	static constexpr int REDUCTION_LIMIT = 2;
	static int visited_nodes;

public:

	Boardstate boardstate;
	FastStack boardstate_history;


	Move killer_heuristic[MAX_SEARCHING_DEPTH][2];
	int pv_length[MAX_SEARCHING_DEPTH];
	Move pv_table[MAX_SEARCHING_DEPTH][MAX_SEARCHING_DEPTH]; //searching depth, move index


	void ParseFEN(const std::string& FEN);
	bool ParseMove(const std::string& move_str);
	void ParsePosition(const std::string& position_str);
	void ParsePerfTest(const std::string& go_str);
	void ParseGo(const std::string& go_str);
	void UCI();

	bool IsSquareAttacked(int square, int attack_side);
	bool IsKingAttacked();
	int GetCapturedPiece(Move move);

	std::vector<Move> GetPseudoMoves();
	bool MakePseudoMove(Move move);
	void MakeNullMove();
	void SaveState();
	void RestoreState();

	void PerfTest(int depth);
	int Evaluate();
	void SortMoves(std::vector<Move>& moves, int depth);
	void SortNonQuietMoves(std::vector<Move>& moves);
	int Search(int max_depth, int depth = 0, int alpha = -INT_MAX, int beta = INT_MAX, bool was_null_move = false);
	int Quiescence(int alpha, int beta);

	void PrintBoard();

	Board(const string& FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

};



constexpr int PIECE_LIST_TABLE[2][6] =
{
	{WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING},
	{BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING}
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

constexpr int PIECE_SIDE_TABLE[12] = { WHITE, WHITE, WHITE, WHITE, WHITE, WHITE, BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };
constexpr int CASTLE_PERMISSION_REQUIREMENT_TABLE[2][2] = { {WK, WQ}, {BK, BQ} }; //[side][castle_type]
constexpr U64 CASTLE_GET_OCCUPANCY_MASK_TABLE[2][2] = { {6917529027641081856, 1008806316530991104}, {96, 14} }; //[side][castle_type]


constexpr int PIECE_MATERIAL_VALUE_TABLE[12] = { 100, 330, 345, 500, 975, 100000, 100, 330, 345, 500, 975, 100000 };
constexpr int PIECE_POSITIONAL_VALUE_TABLE[12][64] =
{
	{//white pawn
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	},
	{//white knight
		-50,-40,-30,-30,-30,-30,-40,-50,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-40,-20,  0,  5,  5,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50
	},
	{//white bishop
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10, 15,  0,  0,  0,  0, 15,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	},
	{//white rook
		 15, 15, 15, 15, 15, 15, 15, 15,
		 15, 15, 15, 15, 15, 15, 15, 15,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  0,  0,  8,  8,  0,  0,  0
	},
	{//whtie queen
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},
	{//white king
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0,
	     0,  0,  0,  0,  0,  0,  0,  0
	},
	{//black pawn
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0,
		 0,  0,  0,  0,  0,  0,  0,  0
	},
	{//black knight
		-50,-40,-30,-30,-30,-30,-40,-50
		-40,-20,  0,  5,  5,  0,-20,-40,
		-30,  5, 10, 15, 15, 10,  5,-30,
		-30,  0, 15, 20, 20, 15,  0,-30,
		-30,  5, 15, 20, 20, 15,  5,-30,
		-30,  0, 10, 15, 15, 10,  0,-30,
		-40,-20,  0,  0,  0,  0,-20,-40,
		-50,-40,-30,-30,-30,-30,-40,-50,
	},
	{//black bishop
		-20,-10,-10,-10,-10,-10,-10,-20,
		-10, 15,  0,  0,  0,  0, 15,-10,
		-10, 10, 10, 10, 10, 10, 10,-10,
		-10,  0, 10, 10, 10, 10,  0,-10,
		-10,  5,  5, 10, 10,  5,  5,-10,
		-10,  0,  5, 10, 10,  5,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10,-10,-10,-10,-10,-20
	},
	{//black rook
		  0,  0,  0,  8,  8,  0,  0,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		  0,  5,  7, 10, 10,  7,  5,  0,
		 15, 15, 15, 15, 15, 15, 15, 15,
		 15, 15, 15, 15, 15, 15, 15, 15
	},
	{//black queen
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		 -5,  0,  0,  0,  0,  0,  0, -5,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},
	{//black king
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0,
		0,  0,  0,  0,  0,  0,  0,  0
	},
};


constexpr int PIECE_PAWN_POSITIONAL_VALUE_TABLE[2][2][64] =
{
	{
		{//white pawn early game
			 0,  0,  0,  0,  0,  0,  0,  0,
			35, 35, 37, 50, 50, 37, 35, 35,
			25, 25, 27, 30, 30, 27, 25, 25,
			12, 12, 15, 25, 25, 15, 12, 12,
			 5,  5,  5, 20, 20,  5,  5,  5,
			 5,  0, -8,  0,  0,-10,  0,  5,
			 5, 10,  0,-20,-20,  0, 10,  5,
			 0,  0,  0,  0,  0,  0,  0,  0
		},
		{//white pawn late game
			 0,  0,  0,  0,  0,  0,  0,  0,
			50, 50, 50, 50, 50, 50, 50, 50,
			40, 40, 40, 40, 40, 40, 40, 40,
			30, 30, 30, 30, 30, 30, 30, 30,
			20, 20, 20, 20, 20, 20, 20, 20,
			10, 10, 10, 10, 10, 10, 10, 10,
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0
		}
	},
	{
		{//black pawn early game
			 0,  0,  0,  0,  0,  0,  0,  0,
			 5, 10,  0,-20,-20,  0, 10,  5,
			 5,  0, -8,  0,  0,-10,  0,  5,
			 5,  5,  5, 20, 20,  5,  5,  5,
			12, 12, 15, 25, 25, 15, 12, 12,
			25, 25, 27, 30, 30, 27, 25, 25,
			35, 35, 37, 50, 50, 37, 35, 35,
			 0,  0,  0,  0,  0,  0,  0,  0
		},
		{//black pawn late game
			 0,  0,  0,  0,  0,  0,  0,  0,
			 0,  0,  0,  0,  0,  0,  0,  0,
			10, 10, 10, 10, 10, 10, 10, 10,
			20, 20, 20, 20, 20, 20, 20, 20,
			30, 30, 30, 30, 30, 30, 30, 30,
			40, 40, 40, 40, 40, 40, 40, 40,
			50, 50, 50, 50, 50, 50, 50, 50,
			 0,  0,  0,  0,  0,  0,  0,  0,
		}
	}
};

constexpr int PIECE_KING_POSITIONAL_VALUE_TABLE[2][2][64] =
{
	{
		{//WHITE KING EARLY GAME
		    -25,-25,-25,-25,-25,-25,-25,-25,
		    -25,-25,-25,-25,-25,-25,-25,-25,
		    -25,-25,-25,-25,-25,-25,-25,-25,
		    -20,-20,-20,-25,-25,-20,-20,-20,
		    -15,-15,-15,-25,-25,-15,-15,-15,
		    -10,-10,-10,-20,-20,-10,-10,-10,
		    -10,-10,  0,-15,-15,  0,-10,-10,
		     20, 30,  0,  0,  0,  0, 30, 20
	    },
	    {//WHITE KING END GAME
			-35,-20,-20,-20,-20,-20,-20,-35,
			-20,-15,-10,-10,-10,-10,-15,-20,
			-20,-10, 10, 10, 10, 10,-10,-20,
			-20,-10, 10, 12, 12, 10,-10,-20,
			-20,-10, 10, 12, 12, 10,-10,-20,
			-20,-10, 10, 10, 10, 10,-10,-20,
			-20,-15,-10,-10,-10,-10,-15,-20,
			-35,-20,-20,-20,-20,-20,-20,-35
	    }
	},
	{
		{//BLACK KING EARLY GAME
		     20, 30,  0,  0,  0,  0, 30, 20,
		    -10,-10,  0,-15,-15,  0,-10,-10,
		    -10,-10,-10,-20,-20,-10,-10,-10,
		    -15,-15,-15,-25,-25,-15,-15,-15,
		    -20,-20,-20,-25,-25,-20,-20,-20,
		    -25,-25,-25,-25,-25,-25,-25,-25,
		    -25,-25,-25,-25,-25,-25,-25,-25,
		    -25,-25,-25,-25,-25,-25,-25,-25
	    },
	    {//BLACK KING END GAME
	    	-35,-20,-20,-20,-20,-20,-20,-35,
	    	-20,-15,-10,-10,-10,-10,-15,-20,
	    	-20,-10, 10, 10, 10, 10,-10,-20,
	    	-20,-10, 10, 12, 12, 10,-10,-20,
	    	-20,-10, 10, 12, 12, 10,-10,-20,
	    	-20,-10, 10, 10, 10, 10,-10,-20,
	    	-20,-15,-10,-10,-10,-10,-15,-20,
	    	-35,-20,-20,-20,-20,-20,-20,-35
	    }
	}
};