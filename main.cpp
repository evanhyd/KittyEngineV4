#include "bitboard.h"
#include "piece_attack_table.h"
#include "board.h"
#include <iostream>
#include <fstream>

using namespace std;


int main()
{
	InitRankMaskTable();
	InitFileMaskTable();

	InitLeaperAttackTable();
	InitSliderAttackTable();
	InitIsolatedPawnMaskTable();
	InitPassedPawnMaskTable();
	Boardstate::InitZobristKeys();

	//r3r1k1/1pp2ppp/2p1b3/p3P3/P1P4q/1nB1Q3/4BPPP/1R3RK1 b - - 1 18 
	Board board;
	board.UCI();
}