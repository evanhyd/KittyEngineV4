#include "bitboard.h"
#include <iostream>
#include <intrin.h>

using std::cout;
using std::cin;


int Bitboard::GetBit(int square) const
{
	return this->bitboard >> square & 1;
}

void Bitboard::SetBit(int square)
{
	this->bitboard |= 1ull << square;
}

void Bitboard::ClearBit(int square)
{
	this->bitboard &= ~(1ull << square);
}

void Bitboard::PopBit()
{
	this->bitboard &= this->bitboard - 1;
}

void Bitboard::FlipBit(int square)
{
	this->bitboard ^= 1ull << square;
}

int Bitboard::CountBit() const
{
	return static_cast<int>(__popcnt64(this->bitboard));
}

int Bitboard::GetLeastSigBit() const
{
	if (this->bitboard == 0) return -1;
	return static_cast<int>(__popcnt64((this->bitboard ^ (this->bitboard-1))) - 1);
}

void Bitboard::PrintBitboard() const
{
	for (int rank = 0; rank < 8; ++rank)
	{
		cout << 8 - rank << "  ";
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			cout << this->GetBit(square)<<" ";
		}
		cout << '\n';
	}
	cout << "   a b c d e f g h\n";
}


Bitboard::operator U64() const
{
	return this->bitboard;
}

Bitboard::operator U64&()
{
	return this->bitboard;
}


Bitboard::Bitboard() : bitboard(0)
{
}

Bitboard::Bitboard(U64 new_bitboard) : bitboard(new_bitboard)
{
}