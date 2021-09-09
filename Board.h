#pragma once
#include "bitboard.h"
#include "boardstate_stack.h"
#include "move.h"
#include "timer.h"
#include "transposition.h"
#include "NeuralNetwork.h"
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

enum : int
{
	WHITE_KING_SIDE_CASTLE = 0b0001, WHITE_QUEEN_SIDE_CASTLE = 0b0010, BLACK_KING_SIDE_CASTLE = 0b0100, BLACK_QUEEN_SIDE_CASTLE = 0b1000 
};

enum : int
{
	KING_SIDE, QUEEN_SIDE
};


char PieceToAscii(int piece);
char PieceToFen(int piece);
int FenToPiece(char FEN);
int FenToPieceType(char FEN);


class Board
{
	//searching
	static constexpr int MAX_SEARCHING_DEPTH = 30;
	static constexpr size_t BOARDSTATE_STACK_SIZE = 256;
	static constexpr int NULL_MOVE_DEPTH_REQUIRED = 1;
	static constexpr int NULL_MOVE_PIECE_REQUIRED = 2;
	static constexpr int NULL_MOVE_DEPTH_REDUCTION = 3;
	static constexpr int LATE_MOVE_SEARCHED_REQUIRED = 4; //change this value if the engine failed to find good moves
	static constexpr int LATE_MOVE_DEPTH_REDUCTION = 3;
	static constexpr size_t REPEATED_POSITION_SIZE = 262144; //power of 2 for fast modulo
	static constexpr size_t TRANSPOSITION_TABLE_SIZE = 8388608; //power of 2 for fast modulo


	//evaluation
	static constexpr int STACKED_PAWN_PENALTY = 10;
	static constexpr int ISOLATED_PAWN_PENALTY = 15;
	static constexpr int PASSED_PAWN_BONUS_TABLE[2][8] =
	{
		{0, 100, 60, 35, 20, 10, 5, 0},
		{0, 5, 10, 20, 35, 60, 100, 0}
	};

	static constexpr int SEMI_OPEN_FILE_BONUS = 10;
	static constexpr int OPEN_FILE_BONUS = 15;
	static constexpr int BISHOP_PAIR_BONUS = 50;
	static constexpr int PIECE_MOBILITY_BONUS_TABLE[12] = { 0, 4, 3, 2, 0, 0, 0, 4, 3, 2, 0, 0 };

	static constexpr int MATE_SCORE = -20000;
	static constexpr int PIECE_MATERIAL_VALUE_TABLE[12] = { 100, 330, 345, 500, 975, 0, 100, 330, 345, 500, 975, 0 };
	static constexpr int PIECE_POSITIONAL_VALUE_TABLE[12][64] =
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
			- 40,-20,  0,  5,  5,  0,-20,-40,
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


	static constexpr int PIECE_PAWN_POSITIONAL_VALUE_TABLE[2][2][64] =
	{
		{
			{//white pawn early game
				 0,  0,  0,  0,  0,  0,  0,  0,
				22, 24, 26, 28, 28, 26, 24, 22,
				19, 21, 23, 25, 25, 23, 21, 19,
				12, 12, 15, 22, 22, 15, 12, 12,
				 5,  5,  0, 20, 20,  5,  5,  5,
				 5,  0, -8,  0,  0,-10,  0,  5,
				 5, 10,  0,-20,-20,  0, 10,  5,
				 0,  0,  0,  0,  0,  0,  0,  0
			},
			{//white pawn late game
				 0,  0,  0,  0,  0,  0,  0,  0,
				24, 26, 28, 30, 30, 28, 26, 24,
				19, 21, 23, 25, 25, 23, 21, 19,
				14, 16, 18, 20, 20, 18, 16, 14,
				 9, 11, 13, 15, 15, 13, 11,  9,
				 4,  6,  8, 10, 10,  8,  6,  4,
				 0,  0,  0,  0,  0,  0,  0,  0,
				 0,  0,  0,  0,  0,  0,  0,  0
			}
		},
		{
			{//black pawn early game
				 0,  0,  0,  0,  0,  0,  0,  0,
				 5, 10,  0,-20,-20,  0, 10,  5,
				 5,  0, -8,  0,  0,-10,  0,  5,
				 5,  5,  0, 20, 20,  5,  5,  5,
				12, 12, 15, 22, 22, 15, 12, 12,
				19, 21, 23, 25, 25, 23, 21, 19,
				22, 24, 26, 28, 28, 26, 24, 22,
				 0,  0,  0,  0,  0,  0,  0,  0
			},
			{//black pawn late game
				 0,  0,  0,  0,  0,  0,  0,  0,
				 0,  0,  0,  0,  0,  0,  0,  0,
				 4,  6,  8, 10, 10,  8,  6,  4,
				 9, 11, 13, 15, 15, 13, 11,  9,
				14, 16, 18, 20, 20, 18, 16, 14,
				19, 21, 23, 25, 25, 23, 21, 19,
				24, 26, 28, 30, 30, 28, 26, 24,
				 0,  0,  0,  0,  0,  0,  0,  0
			}
		}
	};

	static constexpr int PIECE_KING_POSITIONAL_VALUE_TABLE[2][2][64] =
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


	//Neural Network configuration
	//bitboards + enpassant + castling permission + side to move
	static const std::vector<int> MODEL_TOPOLOGY;
	static constexpr const char* MODEL_FILE_NAME = "kitty_brain.nn";
	static constexpr double MODEL_MAX_SCORE = 20000.0;
	static constexpr int MODEL_THINKING_TIME_MS_PER_MOVE = 3000;

public:

	Boardstate boardstate;
	BoardstateStack boardstate_history;

	int visited_nodes;
	int pv_length[MAX_SEARCHING_DEPTH];
	Move pv_table[MAX_SEARCHING_DEPTH][MAX_SEARCHING_DEPTH]; //searching depth, move index
	Move killer_heuristic[MAX_SEARCHING_DEPTH][2];

	bool* repeated_position;
	Transposition* transposition_table;

	NeuralNetwork model;
	bool neural_network_evaluation = false;

	Timer timer;



	void ParseFEN(const std::string& FEN);
	bool ParseMove(const std::string& move_str);
	void ParsePosition(const std::string& position_str);
	void ParsePerfTest(const std::string& go_str);
	void ParseGo(const std::string& go_str);
	void ParseTrain(const std::string& train_str);
	void UCI();

	bool IsSquareAttacked(int square, int attack_side);
	bool IsKingAttacked();
	int GetCapturedPiece(Move move);

	std::vector<Move> GetPseudoMoves();
	bool MakePseudoMove(Move move);
	void MakeNullMove();
	void SaveState();
	void RestoreState();

	bool IsMaterialSufficient(int pawn_num, int minor_num, int major_num);
	int Evaluate();
	int NeuralNetworkEvaluate();
	void NeuralNetworkUpdate(int winning_side);


	void PerfTest(int depth);
	void SortMoves(std::vector<Move>& moves, int depth);
	void SortNonQuietMoves(std::vector<Move>& moves);
	int Search(int max_depth, int depth, int alpha, int beta, bool null_move_reduced = false);
	int Quiescence(int alpha, int beta);

	void PrintBoard();

	Board();
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
constexpr int CASTLE_PERMISSION_REQUIREMENT_TABLE[2][2] = { {WHITE_KING_SIDE_CASTLE, WHITE_QUEEN_SIDE_CASTLE}, {BLACK_KING_SIDE_CASTLE, BLACK_QUEEN_SIDE_CASTLE} }; //[side][castle_type]
constexpr U64 CASTLE_GET_OCCUPANCY_MASK_TABLE[2][2] = { {6917529027641081856, 1008806316530991104}, {96, 14} }; //[side][castle_type]