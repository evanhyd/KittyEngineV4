#include "move.h"
#include "board.h"
#include <iostream>

using std::cout;

int Move::GetSource() const
{
	return this->move & GET_SQUARE_MASK;
}
int Move::GetDest() const
{
	return this->move >> 6 & GET_SQUARE_MASK;
}
int Move::GetMovedPiece() const
{
	return this->move >> 12 & GET_PIECE;
}
int Move::GetPromotedPieceType() const
{
	return this->move >> 16 & GET_PIECE;
}
bool Move::IsCapture() const
{
	return this->move & CAPTURE_FLAG;
}
bool Move::IsEnpassant() const
{
	return this->move & ENPASSANT_FLAG;
}
bool Move::IsDoublePush() const
{
	return this->move & DOUBLE_PUSH_FLAG;
}
bool Move::IsCastling() const
{
	return this->move & CASTLING_FLAG;
}
bool Move::IsQuietMove() const
{
	return !(this->move & VIOLENT_MOVE_FLAG);
}
bool Move::IsMoveEmpty() const
{
	return this->move == 0;
}
void Move::Clear()
{
	this->move = 0;
}
void Move::PrintMove() const
{
	cout << SQUARE_STR_TABLE[GetSource()] << SQUARE_STR_TABLE[GetDest()];
	int promoted_piece = GetPromotedPieceType();
	if (promoted_piece) cout << PieceToFen(promoted_piece);
}

Move::operator int() const
{
	return this->move;
}
Move::operator int&()
{
	return this->move;
}

Move::Move() : move(0)
{
}
Move::Move(int new_move) : move(new_move)
{
}
Move::Move(int source_square, int dest_square, int piece, int promoted_piece_type, int flag) : 
	move(source_square | dest_square << 6 | piece << 12 | promoted_piece_type << 16 | flag)
{
}