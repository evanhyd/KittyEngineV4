#include "bitboard.h"
#include "magic.h"
#include "piece_attack_table.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

/*
using std::cin;
using std::cout;
using std::cerr;

Bitboard BISHOP_MAGIC_NUM_TABLE[64] = {};
Bitboard ROOK_MAGIC_NUM_TABLE[64] = {};

U64 GenerateMagicNumCandidate()
{
	return GenerateRandom64() & GenerateRandom64() & GenerateRandom64();
}


U64 FindMagicNum(int square, bool is_bishop)
{
	int relevant_bits = (is_bishop ? BISHOP_RELEVANT_BITS_TABLE[square] : ROOK_RELEVANT_BITS_TABLE[square]);
	int occupancies_variation = 1 << relevant_bits;

	//2 ^ 12, where 12 is the most possible attacking squares for bishop and rook on fly
	Bitboard *occupancies = new Bitboard[occupancies_variation]();
	Bitboard *attacks = new Bitboard[occupancies_variation]();
	Bitboard *used_attacks = new Bitboard[occupancies_variation]();
	Bitboard attack_mask = (is_bishop ? MaskBishopAttack(square) : MaskRookAttack(square));
	
	for (int index = 0; index < occupancies_variation; ++index)
	{
		//initialize all the possible occupancies variations
		occupancies[index] = MaskOccupancy(attack_mask, index);

		//mask all the attacks on fly based on all occupancies variations
		if (is_bishop) attacks[index] = MaskBishopAttackOnFly(square, occupancies[index]);
		else attacks[index] = MaskRookAttackOnFly(square, occupancies[index]); 
	}


	while (true)
	{
		U64 magic_num_candidate = GenerateMagicNumCandidate();

		//skip inappropriate magicn num to speed up
		if (Bitboard((attack_mask * magic_num_candidate) & 0xff00000000000000).CountBit() < 6) continue;

		//test magic num candidate
		std::fill(used_attacks, used_attacks + occupancies_variation, 0);
		bool failed = false;
		for (int index = 0; index < occupancies_variation; ++index)
		{
			int magic_index = int((occupancies[index] * magic_num_candidate) >> (64 - relevant_bits));

			//if magic index works
			if (used_attacks[magic_index] == 0) used_attacks[magic_index] = attacks[index];
			else if (used_attacks[magic_index] != attacks[index])
			{
			    failed = true;
				break;
			}
		}

		if (!failed)
		{
			delete[] occupancies;
			delete[] attacks;
			delete[] used_attacks;
			return magic_num_candidate;
		}
	}
}


void InitMagicNum()
{
	for (int square = 0; square < 64; ++square)
	{
		BISHOP_MAGIC_NUM_TABLE[square] = FindMagicNum(square, true);
		ROOK_MAGIC_NUM_TABLE[square] = FindMagicNum(square, false);
	}
}*/