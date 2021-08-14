#pragma once
#include "bitboard.h"


constexpr int BISHOP_RELEVANT_BITS_TABLE[64] =
{
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

constexpr int ROOK_RELEVANT_BITS_TABLE[64] =
{
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};


const Bitboard FILTER_A_FILE_MASK = 18374403900871474942;
const Bitboard FILTER_H_FILE_MASK = 9187201950435737471;
const Bitboard FILTER_AB_FILE_MASK = 18229723555195321596;
const Bitboard FILTER_GH_FILE_MASK = 4557430888798830399;

extern Bitboard PAWN_ATTACK_TABLE[2][64];
extern Bitboard KNIGHT_ATTACK_TABLE[64];
extern Bitboard KING_ATTACK_TABLE[64];

extern Bitboard BISHOP_ATTACK_MASK_TABLE[64];
extern Bitboard ROOK_ATTACK_MASK_TABLE[64];
extern Bitboard BISHOP_ATTACK_TABLE[64][512]; //[square][magic index]
extern Bitboard ROOK_ATTACK_TABLE[64][4096];  //[square][magic index]

Bitboard MaskPawnAttack(int side, int square);
Bitboard MaskKnightAttack(int square);
Bitboard MaskKingAttack(int square);
void InitLeaperAttackTable();


Bitboard MaskOccupancy(Bitboard attack_mask, int index_filter_mask);
Bitboard MaskBishopAttack(int square);
Bitboard MaskRookAttack(int square);
Bitboard MaskBishopAttackOnFly(int square, Bitboard occupancy);
Bitboard MaskRookAttackOnFly(int square, Bitboard occupancy);
void InitSliderAttackTable();



//use these functions to access the real attack table
Bitboard GetPawnAttackExact(int attack_side, int square);
Bitboard GetKnightAttackExact(int square);
Bitboard GetKingAttackExact(int square);
Bitboard GetBishopAttackExact(int square, Bitboard occupancy);
Bitboard GetRookAttackExact(int square, Bitboard occupancy);
Bitboard GetQueenAttackExact(int square, Bitboard occupancy);




