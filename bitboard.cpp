#include "bitboard.h"
#include <iostream>
#include <intrin.h>

using std::cout;
using std::cin;


U64 RANK_MASK_TABLE[64] = {};
U64 FILE_MASK_TABLE[64] = {};
U64 RANK_NOT_MASK_TABLE[64] = {};
U64 FILE_NOT_MASK_TABLE[64] = {};

void InitRankMaskTable()
{
	U64 rank_mask = 0b11111111ull;
	for (int rank = 0; rank < 8; ++rank)
	{
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;
			RANK_MASK_TABLE[square] = rank_mask;
			RANK_NOT_MASK_TABLE[square] = ~rank_mask;
		}

		rank_mask <<= 8;
	}
}
void InitFileMaskTable()
{
	U64 file_mask = (1ull | 1ull << 8 | 1ull << 16 | 1ull << 24 | 1ull << 32 | 1ull << 40 | 1ull << 48 | 1ull << 56);
	for (int file = 0; file < 8; ++file)
	{
		for (int rank = 0; rank < 8; ++rank)
		{
			int square = rank * 8 + file;
			FILE_MASK_TABLE[square] = file_mask;
			FILE_NOT_MASK_TABLE[square] = ~file_mask;
		}

		file_mask <<= 1;
	}
}

int GetRank(int square)
{
	return square >> 3;
}
int GetFile(int square)
{
	return square & (8-1);
}
int ToRank(char c)
{
	return 8 - (c - '0');
}
int ToFile(char c)
{
	return tolower(c) - 'a';
}


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