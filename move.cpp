#include "move.h"
#include "board.h"
#include <iostream>

using std::cout;

int Move::GetSource()
{
	return this->move & 0x3f;
}
int Move::GetDest()
{
	return this->move >> 6 & 0x3f;
}
int Move::GetPiece()
{
	return this->move >> 12 & 0xf;
}
int Move::GetPromotedPieceType()
{
	return this->move >> 16 & 0xf;
}
bool Move::IsCapture()
{
	return this->move & 0x100000;
}
bool Move::IsDoublePush()
{
	return this->move & 0x200000;
}
bool Move::IsEnpassant()
{
	return this->move & 0x400000;
}
bool Move::IsCastling()
{
	return this->move & 0x800000;
}
bool Move::IsEmpty()
{
	return this->move == 0;
}
void Move::Clear()
{
	this->move = 0;
}
void Move::PrintMove()
{
	cout << SQUARE_STR_TABLE[GetSource()] << SQUARE_STR_TABLE[GetDest()];
	int promoted_piece = GetPromotedPieceType();
	if (promoted_piece) cout << PieceToFen(promoted_piece);
}

Move::Move(int source_square, int dest_square, int piece, int promoted_piece_type, int flag) : 
	move(source_square | dest_square << 6 | piece << 12 | promoted_piece_type << 16 | flag)
{
}