#include "bitboard.h"
#include "piece_attack_table.h"
#include "random.h"
#include "board.h"
#include <iostream>
#include <unordered_map>
#include <fstream>

using namespace std;



int main()
{
	cin.tie(nullptr)->sync_with_stdio(false);
	InitLeaperAttackTable();
	InitSliderAttackTable();

	Board board;
	
	board.ParsePosition("position fen rnbqkbnr/p4ppp/8/Ppppp3/3P1P2/8/1PP1P1PP/RNBQKBNR w KQkq b6 0 5");

	board.PrintBoard();
}