#include "bitboard.h"
#include "piece_attack_table.h"
#include "random.h"
#include "board.h"
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
	vector<Move> moves = board.GenerateMoves(board.boardstate.side_to_move);
	for (Move move : moves)
	{
		move.PrintMove();
		cout << '\n';
	}

}