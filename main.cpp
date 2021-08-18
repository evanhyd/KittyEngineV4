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

	//8/Pk6/8/8/8/8/6Kp/8 b - - 0 1
	Board board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

	for (int i = 1; i <= 6; ++i)
	{
		int node = 0;
		board.PerfTest(i, node);
		cout << node << '\n';
	}

}