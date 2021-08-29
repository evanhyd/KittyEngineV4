#include "bitboard.h"
#include "piece_attack_table.h"
#include "board.h"
#include <iostream>
#include <fstream>

using namespace std;


int main()
{
	InitLeaperAttackTable();
	InitSliderAttackTable();
	Boardstate::InitZobristKeys();

	Board board;
	board.UCI();
}