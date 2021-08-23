#include "move.h"
#include "board.h"
#include <iostream>

using std::cout;

int Move::GetSource() const
{
	return this->move & 0x3f;
}
int Move::GetDest() const
{
	return this->move >> 6 & 0x3f;
}
int Move::GetMovedPiece() const
{
	return this->move >> 12 & 0xf;
}
int Move::GetPromotedPieceType() const
{
	return this->move >> 16 & 0xf;
}
bool Move::IsCapture() const
{
	return this->move & 0x100000;
}
bool Move::IsDoublePush() const
{
	return this->move & 0x200000;
}
bool Move::IsEnpassant() const
{
	return this->move & 0x400000;
}
bool Move::IsCastling() const
{
	return this->move & 0x800000;
}
bool Move::IsMoveEmpty() const
{
	return this->move == 0;
}
int Move::GetMove() const
{
	return this->move;
}
void Move::ClearMove()
{
	this->move = 0;
}
void Move::SetPriority(int new_priority)
{
	this->priority = new_priority;
}
int Move::GetPriority() const
{
	return this->priority;
}
void Move::PrintMove() const
{
	cout << SQUARE_STR_TABLE[GetSource()] << SQUARE_STR_TABLE[GetDest()];
	int promoted_piece = GetPromotedPieceType();
	if (promoted_piece) cout << PieceToFen(promoted_piece);
}

bool Move::operator<(const Move& other)
{
	return this->priority >= other.priority;
}

Move::Move(int source_square, int dest_square, int piece, int promoted_piece_type, int flag) : 
	move(source_square | dest_square << 6 | piece << 12 | promoted_piece_type << 16 | flag), priority(QUIET_MOVE_PRIORITY)
{
}