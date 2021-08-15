#pragma once


using U64 = unsigned long long;
using U32 = unsigned;


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
	static constexpr U64 LOG2_MAGIC = 0x03f6eaf2cd271461;
	static constexpr int LOG2_TABLE[64] =
	{
		0, 58, 1, 59, 47, 53, 2, 60, 39, 48, 27, 54, 33, 42, 3, 61,
		51, 37, 40, 49, 18, 28, 20, 55, 30, 34, 11, 43, 14, 22, 4, 62,
		57, 46, 52, 38, 26, 32, 41, 50, 36, 17, 19, 29, 10, 13, 21, 56,
		45, 25, 31, 35, 16, 9, 12, 44, 24, 15, 8, 23, 7, 6, 5, 63
	};
};
