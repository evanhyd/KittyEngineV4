#pragma once

using U32 = unsigned;
using U64 = unsigned long long;


enum : int
{
	A8, B8, C8, D8, E8, F8, G8, H8,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A1, B1, C1, D1, E1, F1, G1, H1,
	INVALID_SQUARE = 404
};

constexpr const char* const SQUARE_STR_TABLE[64] =
{
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

enum : int
{
	RANK_8, RANK_7, RANK_6, RANK_5, RANK_4, RANK_3, RANK_2, RANK_1
};

enum : int
{
	FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};


class Bitboard
{
private:
	U64 bitboard;


public:

	int GetBit(int square) const;
	void SetBit(int square);
	void ClearBit(int square);
	void PopBit();
	void FlipBit(int square);
	int CountBit() const;
	int GetLeastSigBit() const;
	void PrintBitboard() const;

	operator U64() const;
	operator U64&();

	Bitboard();
	Bitboard(U64 new_bitboard);


private:

	static constexpr int LSB_HASH_TABLE[64] = 
	{
    	0, 47,  1, 56, 48, 27,  2, 60,
       57, 49, 41, 37, 28, 16,  3, 61,
       54, 58, 35, 52, 50, 42, 21, 44,
       38, 32, 29, 23, 17, 11,  4, 62,
       46, 55, 26, 59, 40, 36, 15, 53,
       34, 51, 20, 43, 31, 22, 10, 45,
       25, 39, 14, 33, 19, 30,  9, 24,
       13, 18,  8, 12,  7,  6,  5, 63
	};

	static constexpr U64 DEBRUIJN64 = 0x03f79d71b4cb0a89;
};
