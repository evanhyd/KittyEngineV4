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


constexpr U64 RANK_7_MASK = 65280;
constexpr U64 RANK_6_MASK = 16711680;
constexpr U64 RANK_3_MASK = 280375465082880;
constexpr U64 RANK_2_MASK = 71776119061217280;
constexpr U64 FILE_A_NOT_MASK = 18374403900871474942;
constexpr U64 FILE_H_NOT_MASK = 9187201950435737471;
constexpr U64 FILE_AB_NOT_MASK = 18229723555195321596;
constexpr U64 FILE_GH_NOT_MASK = 4557430888798830399;

extern U64 RANK_MASK_TABLE[64];
extern U64 RANK_NOT_MASK_TABLE[64];
extern U64 FILE_MASK_TABLE[64];
extern U64 FILE_NOT_MASK_TABLE[64];

void InitRankMaskTable();
void InitFileMaskTable();

int GetRank(int square);
int GetFile(int square);
int ToRank(char c);
int ToFile(char c);


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
};
