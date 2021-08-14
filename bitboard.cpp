#include "bitboard.h"
#include <iostream>

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
	int set_bit = 0;
	for (U64 bits = this->bitboard; bits != 0; ++set_bit)
	{
		bits &= (bits - 1);
	};

	return set_bit;
}

int Bitboard::GetLeastSigBit() const
{
	if (this->bitboard == 0) return -1;

	//https://stackoverflow.com/questions/11376288/fast-computing-of-log2-for-64-bit-integers
	U64 least_sig_set_bit = this->bitboard ^ (this->bitboard & (this->bitboard - 1));

	least_sig_set_bit |= least_sig_set_bit >> 1;
	least_sig_set_bit |= least_sig_set_bit >> 2;
	least_sig_set_bit |= least_sig_set_bit >> 4;
	least_sig_set_bit |= least_sig_set_bit >> 8;
	least_sig_set_bit |= least_sig_set_bit >> 16;
	least_sig_set_bit |= least_sig_set_bit >> 32;

	return LOG2_TABLE[(least_sig_set_bit * LOG2_MAGIC) >> 58];
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
	//empty
}