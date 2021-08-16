#include "bitboard.h"
#include "piece_attack_table.h"
#include "random.h"
#include "Board.h"
#include <iostream>
#include <unordered_map>

using std::cout;
using std::cin;

int main()
{
	cin.tie(nullptr)->sync_with_stdio(false);
	InitLeaperAttackTable();
	InitSliderAttackTable();


	Board board;
	board.ParseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	board.GenerateMoves(board.side_to_move);

}