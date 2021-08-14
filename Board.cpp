#include "Board.h"
#include <iostream>
#include <cctype>

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

Board::Board()
{

}