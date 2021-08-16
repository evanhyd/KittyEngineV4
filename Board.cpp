#include "board.h"
#include "piece_attack_table.h"
#include <iostream>
#include <cctype>
#include <vector>

using namespace std;


char PieceToFen(int piece)
{
	switch (piece)
	{
	case WHITE_PAWN:   return 'P';
	case WHITE_KNIGHT: return 'N';
	case WHITE_BISHOP: return 'B';
	case WHITE_ROOK:   return 'R';
	case WHITE_QUEEN:  return 'Q';
	case WHITE_KING:   return 'K';
	case BLACK_PAWN:   return 'p';
	case BLACK_KNIGHT: return 'n';
	case BLACK_BISHOP: return 'b';
	case BLACK_ROOK:   return 'r';
	case BLACK_QUEEN:  return 'q';
	case BLACK_KING:   return 'k';
	default:           return ' ';
	}
}

int FenToPiece(char FEN)
{
	switch (FEN)
	{
	case 'P': return WHITE_PAWN;
	case 'N': return WHITE_KNIGHT;
	case 'B': return WHITE_BISHOP;
	case 'R': return WHITE_ROOK;
	case 'Q': return WHITE_QUEEN;
	case 'K': return WHITE_KING;
	case 'p': return BLACK_PAWN;
	case 'n': return BLACK_KNIGHT;
	case 'b': return BLACK_BISHOP;
	case 'r': return BLACK_ROOK;
	case 'q': return BLACK_QUEEN;
	case 'k': return BLACK_KING;
	default:  return 420;
	}
}

int GetRank(int square)
{
	return 8 - (square >> 3);
}
int GetFile(int square)
{
	return square & 7;
}



void Board::ParseFEN(const string& str)
{
	fill(begin(this->bitboard), end(this->bitboard), 0);
	fill(begin(this->occupancies), end(this->occupancies), 0);


	auto iter = str.begin();
	for (int square = 0; square < 64;)
	{
		char letter = *iter++;
		if (isdigit(letter)) square += letter - '0';
		else if (isalpha(letter)) this->bitboard[FenToPiece(letter)].SetBit(square++);
	}

	for (int piece = WHITE_PAWN; piece <= WHITE_KING; ++piece)
	{
		this->occupancies[WHITE] |= this->bitboard[piece];
	}
	for (int piece = BLACK_PAWN; piece <= BLACK_KING; ++piece)
	{
		this->occupancies[BLACK] |= this->bitboard[piece];
	}
	this->occupancies[BOTH] = this->occupancies[WHITE] | this->occupancies[BLACK];


	this->side_to_move = (*++iter == 'w' ? WHITE : BLACK);

	iter += 2;

	this->castle = 0;
	while (true)
	{
		if (*iter == ' ') break;

		switch (*iter)
		{
		case 'K': this->castle |= WK; break;
		case 'Q': this->castle |= WQ; break;
		case 'k': this->castle |= BK; break;
		case 'q': this->castle |= BQ; break;
		}
		++iter;
	}

	++iter;

	if (*iter != '-')
	{
		int file = *iter - 'a';
		++iter;
		int rank = 8 - (*iter - '0');
		this->enpassant_square = rank * 8 + file;
	}
	else
	{
		this->enpassant_square = INVALID_SQUARE;
	}

	iter += 2;
}


bool Board::IsSquareAttacked(int square, int attack_side)
{
	//pawn
	int pawn = PIECE_LIST_TABLE[attack_side][PAWN];
	if (GetPawnAttackExact(!attack_side, square) & this->bitboard[pawn]) return true;

	int knight = PIECE_LIST_TABLE[attack_side][KNIGHT];
	if (GetKnightAttackExact(square) & this->bitboard[knight]) return true;

	int bishop = PIECE_LIST_TABLE[attack_side][BISHOP];
	if (GetBishopAttackExact(square, this->occupancies[BOTH]) & this->bitboard[bishop]) return true;

	int rook = PIECE_LIST_TABLE[attack_side][ROOK];
	if (GetRookAttackExact(square, this->occupancies[BOTH]) & this->bitboard[rook]) return true;

	int queen = PIECE_LIST_TABLE[attack_side][QUEEN];
	if (GetQueenAttackExact(square, this->occupancies[BOTH]) & this->bitboard[queen]) return true;

	int king = PIECE_LIST_TABLE[attack_side][KING];
	if (GetKingAttackExact(square) & this->bitboard[king]) return true;

	return false;
}


void Board::GenerateMoves(int side)
{
	vector<int> pseudo_moves;

	int pawn = PIECE_LIST_TABLE[side][PAWN];

	//reverse the occupancy, where bit 1 represents an empty space
	Bitboard not_occupancy = ~this->occupancies[BOTH];

	Bitboard pawn_push = (side == WHITE ? this->bitboard[pawn] >> 8 : this->bitboard[pawn] << 8) & not_occupancy;
	for (Bitboard cpy_pawn_push = pawn_push; cpy_pawn_push; cpy_pawn_push.PopBit())
	{
		int dest_square = cpy_pawn_push.GetLeastSigBit();
		int source_square = (side == WHITE ? dest_square + 8 : dest_square - 8);

		/*
		Add the quiet pawn move


		Mask the promotion rank, add the promotion move
		*/
	}
	

	Bitboard double_pawn_push = (side == WHITE ? pawn_push >> 8 : pawn_push << 8) & not_occupancy;
	for (Bitboard cpy_double_pawn_push = double_pawn_push; cpy_double_pawn_push; cpy_double_pawn_push.PopBit())
	{
		int dest_square = cpy_double_pawn_push.GetLeastSigBit();
		int source_square = (side == WHITE ? dest_square + 16 : dest_square - 16);
		/*
	    Add the double pawn moves


		Mask the promotion rank, add the promotion move
		*/
	}


	Bitboard pawn_capture = 0;
	if (side == WHITE) pawn_capture = this->bitboard[pawn] >> 9 & this->occupancies[!side] & FILTER_H_FILE_MASK | this->bitboard[pawn] >> 7 & this->occupancies[!side] & FILTER_A_FILE_MASK;
	else pawn_capture = this->bitboard[pawn] >> 7 & this->occupancies[!side] & FILTER_H_FILE_MASK | this->bitboard[pawn] >> 9 & this->occupancies[!side] & FILTER_A_FILE_MASK;

	while (pawn_capture)
	{
		int dest_square = pawn_capture.GetLeastSigBit();

		//simulate an attack from the being captured side
		for (Bitboard pawn_attack = PAWN_ATTACK_TABLE[!side][dest_square] & this->occupancies[side]; pawn_attack; pawn_attack.PopBit())
		{
			int source_square = pawn_attack.GetLeastSigBit();
			/*
			Add capture moves

			Mask the promotion rank, add the promotion moves
			*/
		}
		pawn_capture.PopBit();
	}


	if (this->enpassant_square != INVALID_SQUARE)
	{
		int dest_square = this->enpassant_square;

		//simulate an attack from the being captured side
		for (Bitboard pawn_attack = PAWN_ATTACK_TABLE[!side][dest_square] & this->occupancies[side]; pawn_attack; pawn_attack.PopBit())
		{
			int source_square = pawn_attack.GetLeastSigBit();
			/*
			* Add the enpassant move
			*
			* mark the enpassant flag
			*/
		}
	}


	int knight = PIECE_LIST_TABLE[side][KNIGHT];
	for (Bitboard knight_bitboard = this->bitboard[knight]; knight_bitboard; knight_bitboard.PopBit())
	{
		int source_square = knight_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard knight_attack = KNIGHT_ATTACK_TABLE[source_square] & ~this->occupancies[side]; knight_attack; knight_attack.PopBit())
		{
			int dest_square = knight_attack.GetLeastSigBit();
			/*
			* Add the knight attack
			*/
		}
	}


	int king = PIECE_LIST_TABLE[side][KING];
	int king_square = this->bitboard[king].GetLeastSigBit();

	for (int castle_type = KING_SIDE; castle_type <= QUEEN_SIDE; ++castle_type)
	{
		//check the occupancy
		if ((this->occupancies[BOTH] & CASTLE_OCCUPANCY_MASK_TABLE[side][castle_type]) == 0)
		{
			//king and the adjacent squares can not be in checked
			int adj_square = (castle_type == KING_SIDE ? king_square + 1 : king_square - 1);
			int dest_square = (castle_type == KING_SIDE ? king_square + 2 : king_square - 2);

			if (!IsSquareAttacked(king_square, !side) && !IsSquareAttacked(adj_square, !side))
			{
				/*
				* Add the castle move
				*/
			}
		}
	}


	int bishop = PIECE_LIST_TABLE[side][BISHOP];
	for (Bitboard bishop_bitboard = this->bitboard[bishop]; bishop_bitboard; bishop_bitboard.PopBit())
	{
		int source_square = bishop_bitboard.GetLeastSigBit();

		for (Bitboard bishop_attack = GetBishopAttackExact(source_square, this->occupancies[BOTH]) & ~this->occupancies[side]; bishop_attack; bishop_attack.PopBit())
		{
			int dest_square = bishop_attack.GetLeastSigBit();
			/*
			* Add the bishop attack
			*/
		}
	}

	int rook = PIECE_LIST_TABLE[side][ROOK];
	for (Bitboard rook_bitboard = this->bitboard[rook]; rook_bitboard; rook_bitboard.PopBit())
	{
		int source_square = rook_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard rook_attack = GetRookAttackExact(source_square, this->occupancies[BOTH]) & ~this->occupancies[side]; rook_attack; rook_attack.PopBit())
		{
			int dest_square = rook_attack.GetLeastSigBit();
			/*
			* Add the rook attack
			*/
		}
	}

	int queen = PIECE_LIST_TABLE[side][QUEEN];
	for (Bitboard queen_bitboard = this->bitboard[queen]; queen_bitboard; queen_bitboard.PopBit())
	{
		int source_square = queen_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard queen_attack = GetQueenAttackExact(source_square, this->occupancies[BOTH]) & ~this->occupancies[side]; queen_attack; queen_attack.PopBit())
		{
			int dest_square = queen_attack.GetLeastSigBit();
			/*
			* Add the queen attack
			*/
		}
	}
}


void Board::PrintBoard()
{
	for (int rank = 0; rank < 8; ++rank)
	{
		cout << "  -------------------------------\n ";
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;

			bool found = false;
			for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
			{
				if (this->bitboard[piece].GetBit(square))
				{
					cout << "| " << PieceToFen(piece) << ' ';
					found = true;
					break;
				}
			}

			if (!found) cout << "|   ";
		}
		cout << "| " << 8 - rank << '\n';
	}

	cout << "  -------------------------------\n   a   b   c   d   e   f   g   h\n";

	cout << "Castle: " << this->castle << '\n';
	cout << "Enpassant: " << this->enpassant_square << '\n';

	if (this->side_to_move == WHITE) cout << "White to move:\n";
	else cout << "Black to move:\n";
}

Board::Board() : move_history(256)
{
}