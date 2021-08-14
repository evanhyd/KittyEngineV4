#include "piece_attack_table.h"
#include "magic.h"
#include "Board.h"
#include <iostream>

Bitboard PAWN_ATTACK_TABLE[2][64] = {};
Bitboard KNIGHT_ATTACK_TABLE[64] = {};
Bitboard KING_ATTACK_TABLE[64] = {};

Bitboard BISHOP_ATTACK_MASK_TABLE[64] = {};
Bitboard ROOK_ATTACK_MASK_TABLE[64] = {};
Bitboard BISHOP_ATTACK_TABLE[64][512] = {}; //square occupancy
Bitboard ROOK_ATTACK_TABLE[64][4096] = {}; // square occupancy

Bitboard MaskPawnAttack(int side, int square)
{
	Bitboard attack = 0, bitboard = 0;
	bitboard.SetBit(square);

	if (side == WHITE)
	{
		attack |= bitboard >> 7 & FILTER_A_FILE_MASK;
		attack |= bitboard >> 9 & FILTER_H_FILE_MASK;
	}
	else
	{
		attack |= bitboard << 7 & FILTER_H_FILE_MASK;
		attack |= bitboard << 9 & FILTER_A_FILE_MASK;
	}

	return attack;
}


Bitboard MaskKnightAttack(int square)
{
	Bitboard attack = 0, bitboard = 0;
	bitboard.SetBit(square);

	attack |= bitboard >> 17 & FILTER_H_FILE_MASK;
	attack |= bitboard >> 15 & FILTER_A_FILE_MASK;
	attack |= bitboard >> 10 & FILTER_GH_FILE_MASK;
	attack |= bitboard >> 6 & FILTER_AB_FILE_MASK;


	attack |= bitboard << 17 & FILTER_A_FILE_MASK;
	attack |= bitboard << 15 & FILTER_H_FILE_MASK;
	attack |= bitboard << 10 & FILTER_AB_FILE_MASK;
	attack |= bitboard << 6 & FILTER_GH_FILE_MASK;

	return attack;
}


Bitboard MaskKingAttack(int square)
{
	Bitboard attack = 0, bitboard = 0;
	bitboard.SetBit(square);

	attack |= bitboard >> 9 & FILTER_H_FILE_MASK;
	attack |= bitboard >> 8;
	attack |= bitboard >> 7 & FILTER_A_FILE_MASK;
	attack |= bitboard >> 1 & FILTER_H_FILE_MASK;


	attack |= bitboard << 9 & FILTER_A_FILE_MASK;
	attack |= bitboard << 8;
	attack |= bitboard << 7 & FILTER_H_FILE_MASK;
	attack |= bitboard << 1 & FILTER_A_FILE_MASK;


	return attack;
}

void InitLeaperAttackTable()
{
	for (int square = 0; square < 64; ++square)
	{
		PAWN_ATTACK_TABLE[WHITE][square] = MaskPawnAttack(WHITE, square);
		PAWN_ATTACK_TABLE[BLACK][square] = MaskPawnAttack(BLACK, square);

		KNIGHT_ATTACK_TABLE[square] = MaskKnightAttack(square);
		KING_ATTACK_TABLE[square] = MaskKingAttack(square);
	}
}




Bitboard MaskOccupancy(Bitboard attack_mask, int index_filter_mask)
{
	Bitboard occupancy = 0;

	int relevant_bits = attack_mask.CountBit();
	for (int index = 0; index < relevant_bits; ++index)
	{
		int square = attack_mask.GetLeastSigBit();
		attack_mask.PopBit();

		if (1 << index & index_filter_mask) occupancy.SetBit(square);
	}

	return occupancy;
}

Bitboard MaskBishopAttack(int square)
{
	Bitboard attack = 0;

	int rank = square >> 3;
	int file = square & 7;

	for (int r = rank + 1, f = file + 1; r <= 6 && f <= 6; ++r, ++f) { attack |= 1ull << (r * 8 + f); }
	for (int r = rank + 1, f = file - 1; r <= 6 && f >= 1; ++r, --f) { attack |= 1ull << (r * 8 + f); }
	for (int r = rank - 1, f = file + 1; r >= 1 && f <= 6; --r, ++f) { attack |= 1ull << (r * 8 + f); }
	for (int r = rank - 1, f = file - 1; r >= 1 && f >= 1; --r, --f) { attack |= 1ull << (r * 8 + f); }

	return attack;
}


Bitboard MaskRookAttack(int square)
{
	Bitboard attack = 0;

	int rank = square >> 3;
	int file = square & 7;

	for (int r = rank + 1, f = file; r <= 6; ++r) { attack |= 1ull << (r * 8 + f); }
	for (int r = rank - 1, f = file; r >= 1; --r) { attack |= 1ull << (r * 8 + f); }
	for (int r = rank, f = file + 1; f <= 6; ++f) { attack |= 1ull << (r * 8 + f); }
	for (int r = rank, f = file - 1; f >= 1; --f) { attack |= 1ull << (r * 8 + f); }

	return attack;
}


Bitboard MaskBishopAttackOnFly(int square, Bitboard occupancy)
{
	Bitboard attack = 0;

	int rank = square >> 3;
	int file = square & 7;

	for (int r = rank + 1, f = file + 1; r <= 7 && f <= 7; ++r, ++f)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	for (int r = rank + 1, f = file - 1; r <= 7 && f >= 0; ++r, --f)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	for (int r = rank - 1, f = file + 1; r >= 0 && f <= 7; --r, ++f)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; --r, --f)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	return attack;
}

Bitboard MaskRookAttackOnFly(int square, Bitboard occupancy)
{
	Bitboard attack = 0;

	int rank = square >> 3;
	int file = square & 7;

	for (int r = rank + 1, f = file; r <= 7; ++r)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	for (int r = rank - 1, f = file; r >= 0; --r)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	for (int r = rank, f = file + 1; f <= 7; ++f)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	for (int r = rank, f = file - 1; f >= 0; --f)
	{
		Bitboard bitboard = 1ull << (r * 8 + f);
		attack |= bitboard;
		if (bitboard & occupancy) break;
	}

	return attack;
}


void InitSliderAttackTable()
{
	for (int square = 0; square < 64; ++square)
	{
		//init bishop && rook attack mask
		BISHOP_ATTACK_MASK_TABLE[square] = MaskBishopAttack(square);
		ROOK_ATTACK_MASK_TABLE[square] = MaskRookAttack(square);

		//really it is just hashing the occupancy into the magic index
		int occupancies_variation = 1 << BISHOP_RELEVANT_BITS_TABLE[square];
		for (int index = 0; index < occupancies_variation; ++index)
		{
			Bitboard attack_mask_and_occupancy = MaskOccupancy(BISHOP_ATTACK_MASK_TABLE[square], index);
			int magic_index = static_cast<int>((attack_mask_and_occupancy * BISHOP_MAGIC_NUM_TABLE[square]) >> (64 - BISHOP_RELEVANT_BITS_TABLE[square]));
			BISHOP_ATTACK_TABLE[square][magic_index] = MaskBishopAttackOnFly(square, attack_mask_and_occupancy);
		}

		occupancies_variation = 1 << ROOK_RELEVANT_BITS_TABLE[square];
		for (int index = 0; index < occupancies_variation; ++index)
		{
			Bitboard attack_mask_and_occupancy = MaskOccupancy(ROOK_ATTACK_MASK_TABLE[square], index);
			int magic_index = static_cast<int>((attack_mask_and_occupancy * ROOK_MAGIC_NUM_TABLE[square]) >> (64 - ROOK_RELEVANT_BITS_TABLE[square]));
			ROOK_ATTACK_TABLE[square][magic_index] = MaskRookAttackOnFly(square, attack_mask_and_occupancy);
		}
	}
}


Bitboard GetPawnAttackExact(int attack_side, int square)
{
	return PAWN_ATTACK_TABLE[attack_side][square];
}
Bitboard GetKnightAttackExact(int square)
{
	return KNIGHT_ATTACK_TABLE[square];
}
Bitboard GetKingAttackExact(int square)
{
	return KING_ATTACK_TABLE[square];
}

Bitboard GetBishopAttackExact(int square, Bitboard occupancy)
{
	//hash occupancy into magic index;
	int magic_index = static_cast<int>(((occupancy & BISHOP_ATTACK_MASK_TABLE[square]) * BISHOP_MAGIC_NUM_TABLE[square]) >> (64 - BISHOP_RELEVANT_BITS_TABLE[square]));

	return BISHOP_ATTACK_TABLE[square][magic_index];
}

Bitboard GetRookAttackExact(int square, Bitboard occupancy)
{
	//hash occupancy into magic index;
	int magic_index = static_cast<int>(((occupancy & ROOK_ATTACK_MASK_TABLE[square]) * ROOK_MAGIC_NUM_TABLE[square]) >> (64 - ROOK_RELEVANT_BITS_TABLE[square]));

	return ROOK_ATTACK_TABLE[square][magic_index];
}

Bitboard GetQueenAttackExact(int square, Bitboard occupancy)
{
	return GetBishopAttackExact(square, occupancy) | GetRookAttackExact(square, occupancy);
}