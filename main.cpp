#include "bitboard.h"
#include "piece_attack_table.h"
#include "board.h"
#include "zobrist.h"
#include <iostream>
#include <fstream>

using namespace std;


int main()
{
	InitLeaperAttackTable();
	InitSliderAttackTable();
	Zobrist::InitZobristKeys();

	Board board;
	board.UCI();
}