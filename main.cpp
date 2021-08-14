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
	board.ParseFEN("2kr3r/p1ppqpb1/bn2Qnp1/3PN3/1p2P3/2N5/PPPBBPPP/R3K2R b KQ - 3 2");
	board.PrintBoard();

	Bitboard attack = GetQueenAttackExact(C4, board.occupancies[BOTH]);
	attack.PrintBitboard();

}