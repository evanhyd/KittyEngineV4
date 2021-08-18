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
	
	board.ParseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	int node = 0;
	board.PerfTest(5, node);
	cout << node;

}