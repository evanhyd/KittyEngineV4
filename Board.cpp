#include "board.h"
#include "piece_attack_table.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cctype>

using namespace std;
using namespace std::chrono;

const std::vector<int> Board::MODEL_TOPOLOGY = { 12 * 64 + 64 + 4 + 1, 30, 30, 30, 1 };

char PieceToAscii(int piece)
{
	switch (piece)
	{
	case WHITE_PAWN:   return 'A';
	case WHITE_KNIGHT: return 'N';
	case WHITE_BISHOP: return 'B';
	case WHITE_ROOK:   return 'R';
	case WHITE_QUEEN:  return 'Q';
	case WHITE_KING:   return 'K';
	case BLACK_PAWN:   return 'v';
	case BLACK_KNIGHT: return 'n';
	case BLACK_BISHOP: return 'b';
	case BLACK_ROOK:   return 'r';
	case BLACK_QUEEN:  return 'q';
	case BLACK_KING:   return 'k';
	default:           return ' ';
	}
}

char PieceToFen(int piece)
{
	switch (piece)
	{
	case WHITE_PAWN:   return 'P';
	case WHITE_KNIGHT: return 'N';
	case WHITE_BISHOP: return 'B';
	case WHITE_ROOK:   return 'R';
	case WHITE_QUEEN:  return 'Q';
	case WHITE_KING:   return 'K';
	case BLACK_PAWN:   return 'p';
	case BLACK_KNIGHT: return 'n';
	case BLACK_BISHOP: return 'b';
	case BLACK_ROOK:   return 'r';
	case BLACK_QUEEN:  return 'q';
	case BLACK_KING:   return 'k';
	default:           return ' ';
	}
}

int FenToPiece(char FEN)
{
	switch (FEN)
	{
	case 'P': return WHITE_PAWN;
	case 'N': return WHITE_KNIGHT;
	case 'B': return WHITE_BISHOP;
	case 'R': return WHITE_ROOK;
	case 'Q': return WHITE_QUEEN;
	case 'K': return WHITE_KING;
	case 'p': return BLACK_PAWN;
	case 'n': return BLACK_KNIGHT;
	case 'b': return BLACK_BISHOP;
	case 'r': return BLACK_ROOK;
	case 'q': return BLACK_QUEEN;
	case 'k': return BLACK_KING;
	default:  return 420;
	}
}

int FenToPieceType(char FEN)
{
	switch (FEN)
	{
	case 'P':
	case 'p':
		return PAWN;
	case 'N':
	case 'n':
		return KNIGHT;
	case 'B':
	case 'b':
		return BISHOP;
	case 'R':
	case 'r':
		return ROOK;
	case 'Q':
	case 'q':
		return QUEEN;
	case 'K':
	case 'k':
		return KING;

	default:
		return 0;
	}
}



void Board::ParseFEN(const string& FEN)
{
	this->boardstate.Clear();
	this->boardstate_history.Clear();
	this->visited_nodes = 0;
	fill_n(&this->killer_heuristic[0][0], sizeof(this->killer_heuristic) / sizeof(this->killer_heuristic[0][0]), 0);
	fill_n(&this->pv_length[0], sizeof(this->pv_length) / sizeof(this->pv_length[0]), 0);
	fill_n(&this->pv_table[0][0], sizeof(this->pv_table) / sizeof(this->pv_table[0][0]), 0);
	fill(this->repeated_position, this->repeated_position + REPEATED_POSITION_SIZE, false);
	

	auto iter = FEN.begin();
	for (int square = 0; square < 64;)
	{
		char letter = *iter++;
		if (isdigit(letter)) square += letter - '0';
		else if (isalpha(letter)) this->boardstate.bitboards[FenToPiece(letter)].SetBit(square++);
	}

	for (int piece = WHITE_PAWN; piece <= WHITE_KING; ++piece)
	{
		this->boardstate.occupancies[WHITE] |= this->boardstate.bitboards[piece];
	}
	for (int piece = BLACK_PAWN; piece <= BLACK_KING; ++piece)
	{
		this->boardstate.occupancies[BLACK] |= this->boardstate.bitboards[piece];
	}
	this->boardstate.occupancies[BOTH] = this->boardstate.occupancies[WHITE] | this->boardstate.occupancies[BLACK];


	this->boardstate.side_to_move = (*++iter == 'w' ? WHITE : BLACK);
	iter += 2;

	while (true)
	{
		if (*iter == ' ') break;

		switch (*iter)
		{
		case 'K': this->boardstate.castle |= WHITE_KING_SIDE_CASTLE; break;
		case 'Q': this->boardstate.castle |= WHITE_QUEEN_SIDE_CASTLE; break;
		case 'k': this->boardstate.castle |= BLACK_KING_SIDE_CASTLE; break;
		case 'q': this->boardstate.castle |= BLACK_QUEEN_SIDE_CASTLE; break;
		}
		++iter;
	}

	++iter;

	if (*iter != '-')
	{
		int file = ToFile(*iter);
		++iter;
		int rank = ToRank(*iter);
		this->boardstate.enpassant_square = rank * 8 + file;
	}


	iter += 2;

	//generate new position key
	this->boardstate.GenerateNewKey();
}

bool Board::ParseMove(const string& move_str)
{
	if (move_str.size() < 4) return false;

	int source_file = ToFile(move_str[0]);
	int source_rank = ToRank(move_str[1]);
	int dest_file = ToFile(move_str[2]);
	int dest_rank = ToRank(move_str[3]);
	int promoted_piece_type = 0;
	if (move_str.size() >= 5) promoted_piece_type = FenToPieceType(move_str[4]);

	int source_square = source_rank * 8 + source_file;
	int dest_square = dest_rank * 8 + dest_file;

	vector<Move> pseudo_moves = GetPseudoMoves();
	for (Move pseudo_move : pseudo_moves)
	{
		if (pseudo_move.GetSource() == source_square && pseudo_move.GetDest() == dest_square && pseudo_move.GetPromotedPieceType() == promoted_piece_type)
		{
			if (MakePseudoMove(pseudo_move)) return true;
			else return false;
		}
	}

	return false;
}


void Board::ParsePosition(const string& position_str)
{
	size_t index = 9;

	if (position_str.substr(index, 8) == "startpos")
	{
		index += 9;
		ParseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	}
	else if (position_str.substr(index, 8) == "kiwipete")
	{
		index += 9;
		ParseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	}
	else if (position_str.substr(index, 7) == "DTZ1033")
	{
		index += 8;
		ParseFEN("8/r6n/8/8/5k2/3K4/7N/3b3Q b - - 0 0");
	}
	else if (position_str.substr(index, 7) == "endgame")
	{
		index += 8;
		ParseFEN("8/P7/8/8/8/4k3/1K6/8 w - - 1 5");
	}
	else if (position_str.substr(index, 3) == "fen")
	{
		index += 4;
		ParseFEN(position_str.substr(index));
	}

	
	if (index < position_str.size())
	{
		//4 bytes assembly level KMP
		//index = strstr(position_str.c_str(), "move") - position_str.c_str();
		index = position_str.find("moves");
		if (index != string::npos)
		{
			index += 5;
			while (index != string::npos)
			{
				++index;
				bool parsed_success = ParseMove(position_str.substr(index));

				if (parsed_success) index = position_str.find(" ", index);
				else break;
			}
		}
	}
}

void Board::ParsePerfTest(const string& perf_str)
{
	size_t index = perf_str.find("depth") + 6;
	if (index < string::npos)
	{
		this->visited_nodes = 0;
		int max_depth = stoi(perf_str.substr(index));

		high_resolution_clock::time_point timer = high_resolution_clock::now();
		PerfTest(max_depth);
		auto duration = duration_cast<milliseconds>(high_resolution_clock::now() - timer);

		cout << "Node: " << this->visited_nodes << '\n';
		cout << "Time: " << duration.count() << " ms\n";
		cout << "Speed: " << this->visited_nodes / duration.count() << " knode/s"<<endl;
	}
}

void Board::ParseGo(const string& go_str)
{
	int max_depth = MAX_SEARCHING_DEPTH;
	long long remaining_time_ms = Timer::DEFAULT_THINKING_TIME_MS;
	long long increment_time_ms = Timer::DEFAULT_INCREMENT_TIME_MS;

	size_t index = go_str.find("infinite");

	if (index != string::npos)
	{
		max_depth = MAX_SEARCHING_DEPTH;
	}
	else if (go_str.find("depth") != string::npos)
	{
		max_depth = stoi(go_str.substr(9));
	}
	else if (go_str.find("movetime") != string::npos)
	{
		remaining_time_ms = stoi(go_str.substr(12));
		increment_time_ms = remaining_time_ms;
	}
	else
	{
		if (this->boardstate.side_to_move == WHITE)
		{
			index = go_str.find("wtime");
			if (index != string::npos) remaining_time_ms = stoi(go_str.substr(index + 6));

			index = go_str.find("winc");
			if (index != string::npos) increment_time_ms = stoi(go_str.substr(index + 5));
		}
		else
		{
			index = go_str.find("btime");
			if (index != string::npos) remaining_time_ms = stoi(go_str.substr(index + 6));

			index = go_str.find("binc");
			if (index != string::npos) increment_time_ms = stoi(go_str.substr(index + 5));
		}
	}

	this->timer.StartTimer(remaining_time_ms, increment_time_ms);

	this->visited_nodes = 0;
	fill_n(&this->killer_heuristic[0][0], sizeof(this->killer_heuristic) / sizeof(this->killer_heuristic[0][0]), 0);
	fill_n(&this->pv_length[0], sizeof(this->pv_length) / sizeof(this->pv_length[0]), 0);
	fill_n(&this->pv_table[0][0], sizeof(this->pv_table) / sizeof(this->pv_table[0][0]), 0);
	
	for (int i = 0; i < TRANSPOSITION_TABLE_SIZE; ++i)
	{
		this->transposition_table[i].Clear();
	}

	Move best_move = 0;

	for (int depth = 1; depth <= max_depth && depth < MAX_SEARCHING_DEPTH;)
	{
		int score = Search(depth, 0, -INT_MAX, INT_MAX);

		if (timer.IsTimeOut()) break;
		else
		{
			cout << "info score cp " << score << " depth " << depth << " nodes " << visited_nodes << " pv ";
			for (int i = 0; i < this->pv_length[0]; ++i)
			{
				pv_table[0][i].PrintMove();
				cout << ' ';
			}
			cout << endl;
			best_move = pv_table[0][0];
			++depth;
		}
	}

	cout << "bestmove ";
	best_move.PrintMove();
	cout << endl;
	if (best_move) MakePseudoMove(best_move);
}

void Board::ParseTrain(const std::string& train_str)
{
	std::vector<Move> pseudo_moves;

	int max_game = stoi(train_str.substr(6));

	for (int game = 0; game < max_game; ++game)
	{
		std::cout << "Training game: " << game << '\n';
		this->ParseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

		while (true)
		{
			this->neural_network_evaluation = !this->neural_network_evaluation;
			bool in_check = IsKingAttacked();
			bool has_legal_move = false;
			pseudo_moves = GetPseudoMoves();
			for (Move pseudo_move : pseudo_moves)
			{
				if (MakePseudoMove(pseudo_move))
				{
					has_legal_move = true;
					break;
				}
			}

			//game over
			if (!has_legal_move)
			{
				if (in_check) NeuralNetworkUpdate(this->boardstate.side_to_move ^ 1);
				else NeuralNetworkUpdate(BOTH);
				break;
			}
			else
			{
				int pawn_num = this->boardstate.bitboards[WHITE_PAWN].CountBit() + this->boardstate.bitboards[BLACK_PAWN].CountBit();
				int minor_num = this->boardstate.bitboards[WHITE_KNIGHT].CountBit() + this->boardstate.bitboards[WHITE_BISHOP].CountBit() + this->boardstate.bitboards[BLACK_KNIGHT].CountBit() + this->boardstate.bitboards[BLACK_BISHOP].CountBit();
				int major_num = this->boardstate.bitboards[WHITE_ROOK].CountBit() + this->boardstate.bitboards[WHITE_QUEEN].CountBit() + this->boardstate.bitboards[BLACK_ROOK].CountBit() + this->boardstate.bitboards[BLACK_QUEEN].CountBit();
				
				if (!IsMaterialSufficient(pawn_num, minor_num, major_num))
				{
					NeuralNetworkUpdate(BOTH);
					break;
				}
			}

			this->ParseGo("go wtime 20000 btime 20000");
			this->PrintBoard();
		}
	}

	this->neural_network_evaluation = false;
}

void Board::UCI()
{
	ios_base::sync_with_stdio(false);

	string command;
	while (true)
	{
		getline(cin, command);
		if (command == "uci")
		{
			cout << "id name KittyEngine\n";
			cout << "id author UnboxTheCat\n";
			cout << "uciok" << endl;
		}
		else if (command.find("isready") != string::npos) cout << "readyok" << endl;
		else if (command.find("position") != string::npos) ParsePosition(command);
		else if (command.find("ucinewgame") != string::npos) ParsePosition("position startpos");
		else if (command.find("perft") != string::npos) ParsePerfTest(command);
		else if (command.find("go") != string::npos) ParseGo(command);
		else if (command.find("takeback") != string::npos) RestoreState();
		else if (command.find("train") != string::npos) ParseTrain(command);
		else if (command.find("quit") != string::npos) break;
		else cout << "Invaild command\a\n";

		PrintBoard();
	}
}


bool Board::IsSquareAttacked(int square, int attack_side)
{
	//pawn
	int pawn = PIECE_LIST_TABLE[attack_side][PAWN];
	if (GetPawnAttackExact(attack_side^1, square) & this->boardstate.bitboards[pawn]) return true;

	int knight = PIECE_LIST_TABLE[attack_side][KNIGHT];
	if (GetKnightAttackExact(square) & this->boardstate.bitboards[knight]) return true;

	int bishop = PIECE_LIST_TABLE[attack_side][BISHOP];
	if (GetBishopAttackExact(square, this->boardstate.occupancies[BOTH]) & this->boardstate.bitboards[bishop]) return true;

	int rook = PIECE_LIST_TABLE[attack_side][ROOK];
	if (GetRookAttackExact(square, this->boardstate.occupancies[BOTH]) & this->boardstate.bitboards[rook]) return true;

	int queen = PIECE_LIST_TABLE[attack_side][QUEEN];
	if (GetQueenAttackExact(square, this->boardstate.occupancies[BOTH]) & this->boardstate.bitboards[queen]) return true;

	int king = PIECE_LIST_TABLE[attack_side][KING];
	if (GetKingAttackExact(square) & this->boardstate.bitboards[king]) return true;

	return false;
}

bool Board::IsKingAttacked()
{
	int king_side = this->boardstate.side_to_move;
	int king = PIECE_LIST_TABLE[king_side][KING];
	int kign_square = this->boardstate.bitboards[king].GetLeastSigBit();
	return IsSquareAttacked(kign_square, king_side^1);
}

int Board::GetCapturedPiece(Move move)
{
	int moved_piece = move.GetMovedPiece();
	int side = PIECE_SIDE_TABLE[moved_piece];
	int dest_square = move.GetDest();

	for (int captured_piece = PIECE_LIST_TABLE[!side][PAWN]; captured_piece <= PIECE_LIST_TABLE[!side][QUEEN]; ++captured_piece)
	{
		if (this->boardstate.bitboards[captured_piece].GetBit(dest_square)) return captured_piece;
	}

	return -1;
}






vector<Move> Board::GetPseudoMoves()
{
	vector<Move> pseudo_moves;
	pseudo_moves.reserve(218);

	int side = this->boardstate.side_to_move;
	int pawn = PIECE_LIST_TABLE[side][PAWN];

	//reverse the occupancy, where bit 1 represents an empty space
	Bitboard not_occupancy = ~this->boardstate.occupancies[BOTH];

	Bitboard pawn_push = (side == WHITE ? this->boardstate.bitboards[pawn] >> 8 : this->boardstate.bitboards[pawn] << 8) & not_occupancy;
	for (Bitboard cpy_pawn_push = pawn_push; cpy_pawn_push; cpy_pawn_push.PopBit())
	{
		int dest_square = cpy_pawn_push.GetLeastSigBit();
		int source_square = (side == WHITE ? dest_square + 8 : dest_square - 8);

		//no need to check for the side, because pawn can not move backward
		if (GetRank(dest_square) == RANK_8 || GetRank(dest_square) == RANK_1)
		{
			for (int promoted_piece_type = KNIGHT; promoted_piece_type <= QUEEN; ++promoted_piece_type)
			{
				pseudo_moves.push_back(Move(source_square, dest_square, pawn, promoted_piece_type));
			}
		}
		else
		{
			pseudo_moves.push_back(Move(source_square, dest_square, pawn));
		}
	}
	

	Bitboard double_pawn_push = (side == WHITE ? (pawn_push & RANK_3_MASK) >> 8 : (pawn_push & RANK_6_MASK) << 8) & not_occupancy;
	while (double_pawn_push)
	{
		int dest_square = double_pawn_push.GetLeastSigBit();
		int source_square = (side == WHITE ? dest_square + 16 : dest_square - 16);
		pseudo_moves.push_back(Move(source_square, dest_square, pawn, 0, Move::DOUBLE_PUSH_FLAG));
		double_pawn_push.PopBit();
	}


	for (Bitboard pawn_bitboard = this->boardstate.bitboards[pawn]; pawn_bitboard; pawn_bitboard.PopBit())
	{
		int source_square = pawn_bitboard.GetLeastSigBit();

		for (Bitboard pawn_attack = PAWN_ATTACK_TABLE[side][source_square] & this->boardstate.occupancies[!side]; pawn_attack; pawn_attack.PopBit())
		{
			int dest_square = pawn_attack.GetLeastSigBit();

			if (GetRank(dest_square) == RANK_8 || GetRank(dest_square) == RANK_1)
			{
				for (int promoted_piece_type = KNIGHT; promoted_piece_type <= QUEEN; ++promoted_piece_type)
				{
					pseudo_moves.push_back(Move(source_square, dest_square, pawn, promoted_piece_type, Move::CAPTURE_FLAG));
				}
			}
			else
			{
				pseudo_moves.push_back(Move(source_square, dest_square, pawn, 0, Move::CAPTURE_FLAG));
			}
		}
	}


	if (this->boardstate.enpassant_square != INVALID_SQUARE)
	{
		int dest_square = this->boardstate.enpassant_square;

		//simulate an attack from the being captured side, the source must be a pawn
		for (Bitboard pawn_attack = PAWN_ATTACK_TABLE[!side][dest_square] & this->boardstate.bitboards[pawn]; pawn_attack; pawn_attack.PopBit())
		{
			int source_square = pawn_attack.GetLeastSigBit();

			//do i need capture flag?
			pseudo_moves.push_back(Move(source_square, dest_square, pawn, 0, Move::ENPASSANT_FLAG));
		}
	}






	int knight = PIECE_LIST_TABLE[side][KNIGHT];
	for (Bitboard knight_bitboard = this->boardstate.bitboards[knight]; knight_bitboard; knight_bitboard.PopBit())
	{
		int source_square = knight_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard knight_attack = GetKnightAttackExact(source_square) & ~this->boardstate.occupancies[side]; knight_attack; knight_attack.PopBit())
		{
			int dest_square = knight_attack.GetLeastSigBit();
			if(this->boardstate.occupancies[!side].GetBit(dest_square)) pseudo_moves.push_back(Move(source_square, dest_square, knight, 0, Move::CAPTURE_FLAG));
			else pseudo_moves.push_back(Move(source_square, dest_square, knight));
		}
	}




	int king = PIECE_LIST_TABLE[side][KING];
	int king_square = this->boardstate.bitboards[king].GetLeastSigBit();

	for (Bitboard king_attack = GetKingAttackExact(king_square) & ~this->boardstate.occupancies[side]; king_attack; king_attack.PopBit())
	{
		int dest_square = king_attack.GetLeastSigBit();
		
		if (this->boardstate.occupancies[!side].GetBit(dest_square)) pseudo_moves.push_back(Move(king_square, dest_square, king, 0, Move::CAPTURE_FLAG));
		else pseudo_moves.push_back(Move(king_square, dest_square, king));
	}

	for (int castle_type = KING_SIDE; castle_type <= QUEEN_SIDE; ++castle_type)
	{
		//check for the permission
		if (this->boardstate.castle & CASTLE_PERMISSION_REQUIREMENT_TABLE[side][castle_type])
		{
			//check the occupancy
			if ((this->boardstate.occupancies[BOTH] & CASTLE_GET_OCCUPANCY_MASK_TABLE[side][castle_type]) == 0)
			{
				//king and the adjacent squares can not be in checked
				int adj_square = (castle_type == KING_SIDE ? king_square + 1 : king_square - 1);
				int dest_square = (castle_type == KING_SIDE ? king_square + 2 : king_square - 2);

				if (!IsSquareAttacked(king_square, side^1) && !IsSquareAttacked(adj_square, side^1))
				{
					pseudo_moves.push_back(Move(king_square, dest_square, king, 0, Move::CASTLING_FLAG));
				}
			}
		}
	}






	int bishop = PIECE_LIST_TABLE[side][BISHOP];
	for (Bitboard bishop_bitboard = this->boardstate.bitboards[bishop]; bishop_bitboard; bishop_bitboard.PopBit())
	{
		int source_square = bishop_bitboard.GetLeastSigBit();

		for (Bitboard bishop_attack = GetBishopAttackExact(source_square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[side]; bishop_attack; bishop_attack.PopBit())
		{
			int dest_square = bishop_attack.GetLeastSigBit();
			
			if (this->boardstate.occupancies[!side].GetBit(dest_square)) pseudo_moves.push_back(Move(source_square, dest_square, bishop, 0, Move::CAPTURE_FLAG));
			else pseudo_moves.push_back(Move(source_square, dest_square, bishop));
		}
	}





	int rook = PIECE_LIST_TABLE[side][ROOK];
	for (Bitboard rook_bitboard = this->boardstate.bitboards[rook]; rook_bitboard; rook_bitboard.PopBit())
	{
		int source_square = rook_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard rook_attack = GetRookAttackExact(source_square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[side]; rook_attack; rook_attack.PopBit())
		{
			int dest_square = rook_attack.GetLeastSigBit();
			
			if (this->boardstate.occupancies[!side].GetBit(dest_square)) pseudo_moves.push_back(Move(source_square, dest_square, rook, 0, Move::CAPTURE_FLAG));
			else pseudo_moves.push_back(Move(source_square, dest_square, rook));
		}
	}





	int queen = PIECE_LIST_TABLE[side][QUEEN];
	for (Bitboard queen_bitboard = this->boardstate.bitboards[queen]; queen_bitboard; queen_bitboard.PopBit())
	{
		int source_square = queen_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard queen_attack = GetQueenAttackExact(source_square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[side]; queen_attack; queen_attack.PopBit())
		{
			int dest_square = queen_attack.GetLeastSigBit();
			
			if (this->boardstate.occupancies[!side].GetBit(dest_square)) pseudo_moves.push_back(Move(source_square, dest_square, queen, 0, Move::CAPTURE_FLAG));
			else pseudo_moves.push_back(Move(source_square, dest_square, queen));
		}
	}

	return pseudo_moves;
}

bool Board::MakePseudoMove(Move move)
{
	SaveState();

	int side = this->boardstate.side_to_move;

	int source_square = move.GetSource();
	int dest_square = move.GetDest();
	int moving_piece = move.GetMovedPiece();

	this->boardstate.bitboards[moving_piece].FlipBit(source_square);
	this->boardstate.bitboards[moving_piece].FlipBit(dest_square);
	this->boardstate.occupancies[side].FlipBit(source_square);
	this->boardstate.occupancies[side].FlipBit(dest_square);
	this->boardstate.HashPiece(moving_piece, source_square);
	this->boardstate.HashPiece(moving_piece, dest_square);

	if (move.IsCapture())
	{
		int captured_piece = GetCapturedPiece(move);
		this->boardstate.bitboards[captured_piece].FlipBit(dest_square);
		this->boardstate.occupancies[!side].FlipBit(dest_square);
		this->boardstate.HashPiece(captured_piece, dest_square);
	}

	int promoted_piece_type = move.GetPromotedPieceType();
	if (promoted_piece_type)
	{
		int promoted_piece = PIECE_LIST_TABLE[side][promoted_piece_type];
		this->boardstate.bitboards[moving_piece].FlipBit(dest_square);
		this->boardstate.bitboards[promoted_piece].FlipBit(dest_square);
		this->boardstate.HashPiece(moving_piece, dest_square);
		this->boardstate.HashPiece(promoted_piece, dest_square);
	}
	
	if (move.IsEnpassant())
	{
		int captured_pawn = PIECE_LIST_TABLE[!side][PAWN];
		int captured_pawn_square = (side == WHITE ? dest_square + 8 : dest_square - 8);

		this->boardstate.bitboards[captured_pawn].FlipBit(captured_pawn_square);
		this->boardstate.occupancies[!side].FlipBit(captured_pawn_square);
		this->boardstate.HashPiece(captured_pawn, captured_pawn_square);
	}

	//unhash the old enpassant square
	if(this->boardstate.enpassant_square != INVALID_SQUARE) this->boardstate.HashEnpassant(this->boardstate.enpassant_square);
	
	if (move.IsDoublePush())
	{
		this->boardstate.enpassant_square = (side == WHITE ? source_square - 8 : source_square + 8);
		this->boardstate.HashEnpassant(this->boardstate.enpassant_square);
	}
	else this->boardstate.enpassant_square = INVALID_SQUARE;

	if (move.IsCastling())
	{
		int rook = PIECE_LIST_TABLE[side][ROOK];
		int rook_source = 0, rook_dest = 0;
		
		switch (dest_square)
		{
		case G1: rook_source = H1; rook_dest = F1; break;
		case C1: rook_source = A1; rook_dest = D1; break;
		case G8: rook_source = H8; rook_dest = F8; break;
		case C8: rook_source = A8; rook_dest = D8; break;
		}

		//flip bit is cheaper
		this->boardstate.bitboards[rook].FlipBit(rook_source);
		this->boardstate.bitboards[rook].FlipBit(rook_dest);
		this->boardstate.occupancies[side].FlipBit(rook_source);
		this->boardstate.occupancies[side].FlipBit(rook_dest);
		this->boardstate.HashPiece(rook, rook_source);
		this->boardstate.HashPiece(rook, rook_dest);
	}

	this->boardstate.HashCastle(this->boardstate.castle);
	this->boardstate.castle &= CASTLING_PERMISSION_FILTER_TABLE[source_square];
	this->boardstate.castle &= CASTLING_PERMISSION_FILTER_TABLE[dest_square];
	this->boardstate.HashCastle(this->boardstate.castle);


	//update the occupancies
	this->boardstate.occupancies[BOTH] = this->boardstate.occupancies[WHITE] | this->boardstate.occupancies[BLACK];

	//update fifty moves rule
	if (moving_piece == WHITE_PAWN || moving_piece == BLACK_PAWN || move.IsCapture() || move.IsEnpassant()) this->boardstate.fifty_moves = 0;
	else ++this->boardstate.fifty_moves;

	//check for check
	bool in_check = IsKingAttacked();

	if (in_check)
	{
		RestoreState();
		return false;
	}
	else
	{
		this->boardstate.side_to_move = this->boardstate.side_to_move^1;
		this->boardstate.HashSideToMove();
		return true;
	}
}

void Board::MakeNullMove()
{
	SaveState();

	//unhash the old enpassant square
	if (this->boardstate.enpassant_square != INVALID_SQUARE) this->boardstate.HashEnpassant(this->boardstate.enpassant_square);
	this->boardstate.enpassant_square = INVALID_SQUARE;

	//update the side to move hash
	this->boardstate.HashSideToMove();
	this->boardstate.side_to_move = this->boardstate.side_to_move^1;
}

//huge bottleneck
void Board::SaveState()
{
	this->repeated_position[this->boardstate.position_key & (REPEATED_POSITION_SIZE - 1)] = true;
	this->boardstate_history.PushBack(this->boardstate);
}
void Board::RestoreState()
{
	this->boardstate = this->boardstate_history.Back();
	this->boardstate_history.PopBack();
	this->repeated_position[this->boardstate.position_key & (REPEATED_POSITION_SIZE - 1)] = false;
}










bool Board::IsMaterialSufficient(int pawn_num, int minor_num, int major_num)
{
	return pawn_num || minor_num > 1 || major_num;
}

int Board::Evaluate()
{
	int score = 0;
	int minor_piece_num[3] = {};
	int major_piece_num[3] = {};

	const Bitboard* this_bitboards = this->boardstate.bitboards;


	for (Bitboard bitboard = this_bitboards[WHITE_KNIGHT]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score += PIECE_MATERIAL_VALUE_TABLE[WHITE_KNIGHT] + PIECE_POSITIONAL_VALUE_TABLE[WHITE_KNIGHT][square];

		//mobility bonus
		Bitboard attack_table = GetKnightAttackExact(square) & ~this->boardstate.occupancies[WHITE];
		score += attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[WHITE_KNIGHT];
	}

	for (Bitboard bitboard = this_bitboards[WHITE_BISHOP]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score += PIECE_MATERIAL_VALUE_TABLE[WHITE_BISHOP] + PIECE_POSITIONAL_VALUE_TABLE[WHITE_BISHOP][square];

		//mobility bonus
		Bitboard attack_table = GetBishopAttackExact(square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[WHITE];
		score += attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[WHITE_BISHOP];
	}

	for (Bitboard bitboard = this_bitboards[WHITE_ROOK]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score += PIECE_MATERIAL_VALUE_TABLE[WHITE_ROOK] + PIECE_POSITIONAL_VALUE_TABLE[WHITE_ROOK][square];

		//mobility bonus
		Bitboard attack_table = GetRookAttackExact(square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[WHITE];
		score += attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[WHITE_ROOK];

		//semi open / open file bonuses
		if ((this_bitboards[WHITE_PAWN] & FILE_MASK_TABLE[square]) == 0)
		{
			if ((this_bitboards[BLACK_PAWN] & FILE_MASK_TABLE[square]) == 0) score += OPEN_FILE_BONUS;
			else score += SEMI_OPEN_FILE_BONUS;
		}
	}

	for (Bitboard bitboard = this_bitboards[WHITE_QUEEN]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score += PIECE_MATERIAL_VALUE_TABLE[WHITE_QUEEN] + PIECE_POSITIONAL_VALUE_TABLE[WHITE_QUEEN][square];

		//mobility bonus
		Bitboard attack_table = GetQueenAttackExact(square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[WHITE];
		score += attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[WHITE_QUEEN];
	}









	for (Bitboard bitboard = this_bitboards[BLACK_KNIGHT]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score -= PIECE_MATERIAL_VALUE_TABLE[BLACK_KNIGHT] + PIECE_POSITIONAL_VALUE_TABLE[BLACK_KNIGHT][square];

		//mobility bonus
		Bitboard attack_table = GetKnightAttackExact(square) & ~this->boardstate.occupancies[BLACK];
		score -= attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[BLACK_KNIGHT];
	}

	for (Bitboard bitboard = this_bitboards[BLACK_BISHOP]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score -= PIECE_MATERIAL_VALUE_TABLE[BLACK_BISHOP] + PIECE_POSITIONAL_VALUE_TABLE[BLACK_BISHOP][square];

		//mobility bonus
		Bitboard attack_table = GetBishopAttackExact(square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[BLACK];
		score -= attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[BLACK_BISHOP];
	}

	for (Bitboard bitboard = this_bitboards[BLACK_ROOK]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score -= PIECE_MATERIAL_VALUE_TABLE[BLACK_ROOK] + PIECE_POSITIONAL_VALUE_TABLE[BLACK_ROOK][square];

		//mobility bonus
		Bitboard attack_table = GetRookAttackExact(square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[BLACK];
		score -= attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[BLACK_ROOK];

		//semi open / open file bonuses
		if ((this_bitboards[BLACK_PAWN] & FILE_MASK_TABLE[square]) == 0)
		{
			if ((this_bitboards[WHITE_PAWN] & FILE_MASK_TABLE[square]) == 0) score -= OPEN_FILE_BONUS;
			else score -= SEMI_OPEN_FILE_BONUS;
		}
	}

	for (Bitboard bitboard = this_bitboards[BLACK_QUEEN]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score -= PIECE_MATERIAL_VALUE_TABLE[BLACK_QUEEN] + PIECE_POSITIONAL_VALUE_TABLE[BLACK_QUEEN][square];

		//mobility bonus
		Bitboard attack_table = GetQueenAttackExact(square, this->boardstate.occupancies[BOTH]) & ~this->boardstate.occupancies[BLACK];
		score -= attack_table.CountBit() * PIECE_MOBILITY_BONUS_TABLE[BLACK_QUEEN];
	}

	minor_piece_num[WHITE] = this_bitboards[WHITE_KNIGHT].CountBit() + this_bitboards[WHITE_BISHOP].CountBit();
	major_piece_num[WHITE] = this_bitboards[WHITE_ROOK].CountBit() + this_bitboards[WHITE_QUEEN].CountBit();

	minor_piece_num[BLACK] = this_bitboards[BLACK_KNIGHT].CountBit() + this_bitboards[BLACK_BISHOP].CountBit();
	major_piece_num[BLACK] = this_bitboards[BLACK_ROOK].CountBit() + this_bitboards[BLACK_QUEEN].CountBit();

	minor_piece_num[BOTH] = minor_piece_num[WHITE] + minor_piece_num[BLACK];
	major_piece_num[BOTH] = major_piece_num[WHITE] + major_piece_num[BLACK];

	
	bool is_end_game = minor_piece_num[this->boardstate.side_to_move ^ 1] + major_piece_num[this->boardstate.side_to_move ^ 1] <= 3;


	for (Bitboard bitboard = this_bitboards[WHITE_PAWN]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score += PIECE_MATERIAL_VALUE_TABLE[WHITE_PAWN] + PIECE_PAWN_POSITIONAL_VALUE_TABLE[WHITE][is_end_game][square];

		//stacked pawns penalty
		if (this_bitboards[WHITE_PAWN] & FILE_MASK_TABLE[square]) score -= STACKED_PAWN_PENALTY;

		//isolated pawn penalty
		if ((this_bitboards[WHITE_PAWN] & ISOLATED_PAWN_MASK_TABLE[square]) == 0) score -= ISOLATED_PAWN_PENALTY;

		//passed pawn bonus
		if ((this_bitboards[BLACK_PAWN] & PASSED_PAWN_MASK_TABLE[WHITE][square]) == 0) score += PASSED_PAWN_BONUS_TABLE[WHITE][GetRank(square)];
	}

	for (Bitboard bitboard = this_bitboards[BLACK_PAWN]; bitboard; bitboard.PopBit())
	{
		int square = bitboard.GetLeastSigBit();
		score -= PIECE_MATERIAL_VALUE_TABLE[BLACK_PAWN] + PIECE_PAWN_POSITIONAL_VALUE_TABLE[BLACK][is_end_game][square];

		//stacked pawns penalty
		if (this_bitboards[BLACK_PAWN] & FILE_MASK_TABLE[square]) score += STACKED_PAWN_PENALTY;

		//isolated pawn penalty
		if ((this_bitboards[BLACK_PAWN] & ISOLATED_PAWN_MASK_TABLE[square]) == 0) score += ISOLATED_PAWN_PENALTY;

		//passed pawn bonus
		if ((this_bitboards[WHITE_PAWN] & PASSED_PAWN_MASK_TABLE[BLACK][square]) == 0) score -= PASSED_PAWN_BONUS_TABLE[BLACK][GetRank(square)];
	}


	//check material sufficiency
	if (!IsMaterialSufficient(this_bitboards[WHITE_PAWN].CountBit() + this_bitboards[BLACK_PAWN].CountBit(), minor_piece_num[BOTH], major_piece_num[BOTH])) return 0;


	//king early/endgame positional bonus
	int white_king_square = this_bitboards[WHITE_KING].GetLeastSigBit();
	score += PIECE_KING_POSITIONAL_VALUE_TABLE[WHITE][is_end_game][white_king_square];
	int black_king_square = this_bitboards[BLACK_KING].GetLeastSigBit();
	score -= PIECE_KING_POSITIONAL_VALUE_TABLE[BLACK][is_end_game][black_king_square];


	//bishop pair compensation
	if (this_bitboards[WHITE_BISHOP].CountBit() >= 2 && this_bitboards[BLACK_BISHOP].CountBit() == 0) score += BISHOP_PAIR_BONUS;
	else if (this_bitboards[BLACK_BISHOP].CountBit() >= 2 && this_bitboards[WHITE_BISHOP].CountBit() == 0) score -= BISHOP_PAIR_BONUS;


	return this->boardstate.side_to_move == WHITE ? score : -score;
}


int Board::NeuralNetworkEvaluate()
{
	vector<double> features(MODEL_TOPOLOGY.front());
	int feature_index = 0;

	for (Bitboard bitboard : this->boardstate.bitboards)
	{
		for (int square = 0; square < 64; ++square)
		{
			features[feature_index] = double(bitboard.GetBit(square));
			++feature_index;
		}
	}

	features[12 * 64 + this->boardstate.enpassant_square] = 1.0;
	features[MODEL_TOPOLOGY.front() - 5] = double(this->boardstate.castle & WHITE_KING_SIDE_CASTLE);
	features[MODEL_TOPOLOGY.front() - 4] = double(this->boardstate.castle & WHITE_QUEEN_SIDE_CASTLE);
	features[MODEL_TOPOLOGY.front() - 3] = double(this->boardstate.castle & BLACK_KING_SIDE_CASTLE);
	features[MODEL_TOPOLOGY.front() - 2] = double(this->boardstate.castle & BLACK_QUEEN_SIDE_CASTLE);
	features[MODEL_TOPOLOGY.front() - 1] = double(this->boardstate.side_to_move);

	this->model.ForwardPropagate(features);
	vector<double> predict = this->model.GetResult();
	int score = int(predict.front() * MODEL_MATE_SCORE);
	return (this->boardstate.side_to_move == WHITE ? score : -score);
}

void Board::NeuralNetworkUpdate(int winning_side)
{
	vector<double> labeled_examples(MODEL_TOPOLOGY.front());
	int example_index = 0;

	const Boardstate* stack = this->boardstate_history.Data();
	for (int i = 0; i < this->boardstate_history.Size(); ++i)
	{
		example_index = 0;

		for (Bitboard bitboard : stack[i].bitboards)
		{
			for (int square = 0; square < 64; ++square)
			{
				labeled_examples[example_index] = bitboard.GetBit(square);
				++example_index;
			}
		}

		labeled_examples[12 * 64 + this->boardstate.enpassant_square] = 1.0;
		labeled_examples[MODEL_TOPOLOGY.front() - 5] = stack[i].castle & WHITE_KING_SIDE_CASTLE;
		labeled_examples[MODEL_TOPOLOGY.front() - 4] = stack[i].castle & WHITE_QUEEN_SIDE_CASTLE;
		labeled_examples[MODEL_TOPOLOGY.front() - 3] = stack[i].castle & BLACK_KING_SIDE_CASTLE;
		labeled_examples[MODEL_TOPOLOGY.front() - 2] = stack[i].castle & BLACK_QUEEN_SIDE_CASTLE;
		labeled_examples[MODEL_TOPOLOGY.front() - 1] = stack[i].side_to_move;

		this->model.ForwardPropagate(labeled_examples);

		if (winning_side == WHITE) this->model.BackPropagate({1.0});
		else if(winning_side == BLACK) this->model.BackPropagate({ -1.0 });
		else this->model.BackPropagate({ 0.0 });
	}

	this->model.SaveNeuralNetwork(MODEL_FILE_NAME);
}








void Board::PerfTest(int depth)
{
	if (depth == 0)
	{
		++this->visited_nodes;
		return;
	}

	vector<Move> pseudo_moves = GetPseudoMoves();

	for (Move move : pseudo_moves)
	{
		if (MakePseudoMove(move))
		{
			PerfTest(depth - 1);
			RestoreState();
		}
	}

	return;
}


void Board::SortMoves(vector<Move>& moves, int depth)
{
	//priority, move
	vector<pair<int, Move>> orders(moves.size());
	for (int i = 0; i < moves.size(); ++i)
	{
		if (moves[i] == this->pv_table[0][depth])
		{
			orders[i].first = Move::PRINCIPAL_VARIATION_PRIORITY;
		}
		else
		{
			if (moves[i].IsCapture()) orders[i].first += Move::CAPTURE_PRIORITY_TABLE[moves[i].GetMovedPiece()][GetCapturedPiece(moves[i])];

			if (moves[i].GetPromotedPieceType() != 0) orders[i].first += Move::PROMOTION_PRIORITY;
			else if (moves[i].IsEnpassant()) orders[i].first += Move::ENPASSANT_PRIORITY;
			else
			{
				if (killer_heuristic[depth][0] == moves[i]) orders[i].first += Move::KILLER_MOVE;
				else if (killer_heuristic[depth][1] == moves[i]) orders[i].first += Move::KILLER_MOVE - 1;
			}
		}
		orders[i].second = moves[i];
	}

	sort(orders.rbegin(), orders.rend());

	for (int i = 0; i < orders.size(); ++i)
	{
		moves[i] = orders[i].second;
	}
}

void Board::SortNonQuietMoves(std::vector<Move>& moves)
{
	//priority, move
	vector<pair<int, Move>> orders(moves.size());
	for (int i = 0; i < moves.size(); ++i)
	{
		if (moves[i].IsCapture()) orders[i].first += Move::CAPTURE_PRIORITY_TABLE[moves[i].GetMovedPiece()][GetCapturedPiece(moves[i])];

		if (moves[i].GetPromotedPieceType() != 0) orders[i].first += Move::PROMOTION_PRIORITY;
		else if (moves[i].IsEnpassant()) orders[i].first += Move::ENPASSANT_PRIORITY;

		orders[i].second = moves[i];
	}

	sort(orders.rbegin(), orders.rend());

	for (int i = 0; i < orders.size(); ++i)
	{
		moves[i] = orders[i].second;
	}
}



int Board::Search(int max_depth, int depth, int alpha, int beta, bool null_move_reduced)
{
	//must before returning
	this->pv_length[depth] = depth;

	int depth_searched_beyond = max_depth - depth;

	//threefold repetition
	if (depth > 0 && this->repeated_position[this->boardstate.position_key & (REPEATED_POSITION_SIZE - 1)]) return 0;

	//fifty move rules
	if (depth > 0 && this->boardstate.fifty_moves == 50) return 0;
	

	//should also check the max_seraching depth to avoid overflow
	if (depth >= max_depth)
	{
		++this->visited_nodes; 
		return Quiescence(alpha, beta);
	}

	
	//look up table
	if (depth > 0)
	{
		int score = Transposition::ProbeHashScore(this->transposition_table, TRANSPOSITION_TABLE_SIZE, this->boardstate.position_key, depth_searched_beyond, alpha, beta, this->boardstate.side_to_move);
		if (score != Transposition::SCORE_EMPTY) return score;
	}


	vector<Move> pseudo_moves = GetPseudoMoves();
	SortMoves(pseudo_moves, depth);

	bool in_check = IsKingAttacked();

	//null move pruning
	if (!null_move_reduced && depth >= NULL_MOVE_DEPTH_REQUIRED && depth_searched_beyond >= NULL_MOVE_DEPTH_REDUCTION && !pseudo_moves.empty() && !in_check)
	{
		int big_piece_num = 0;
		for (int piece = PIECE_LIST_TABLE[this->boardstate.side_to_move][KNIGHT]; piece <= PIECE_LIST_TABLE[this->boardstate.side_to_move][QUEEN]; ++piece)
		{
			big_piece_num += this->boardstate.bitboards[piece].CountBit();
		}

		if (big_piece_num >= NULL_MOVE_PIECE_REQUIRED)
		{
			MakeNullMove();
			int score = -Search(max_depth, depth + NULL_MOVE_DEPTH_REDUCTION, -beta, -beta + 1, true);
			RestoreState();
			if (score >= beta) return beta;
		}
	}

	bool is_pv_node_candidate = false;
	int legal_moves_searched = 0, hash_flag = Transposition::HASH_FLAG_ALPHA;
	for (Move pseudo_move : pseudo_moves)
	{
		if (this->timer.IsTimeOut()) break;
		if (!MakePseudoMove(pseudo_move)) continue;

		int score;

		//first move full search
		if (legal_moves_searched == 0)
		{
			score = -Search(max_depth, depth + 1, -beta, -alpha);
		}
		else
		{
			bool enemy_in_check = IsKingAttacked();

			//late move reduction after seraching all the non-quiet moves, pv moves, and killer moves
			//only execute LMR when this node is less likely to be a pv node candidate
			if (depth_searched_beyond >= LATE_MOVE_DEPTH_REDUCTION && legal_moves_searched >= LATE_MOVE_SEARCHED_REQUIRED && !is_pv_node_candidate &&
				!in_check && !enemy_in_check && pseudo_move.IsQuietMove() && pseudo_move != this->pv_table[0][depth] &&
				pseudo_move != this->killer_heuristic[depth][0] && pseudo_move != this->killer_heuristic[depth][1])
			{
				score = -Search(max_depth, depth + LATE_MOVE_DEPTH_REDUCTION, -alpha - 1, -alpha);
			}
			//if can't use late move reduction, then use pv search
			//if late move reduction raises alpha, then use pv search
			else score = alpha + 1;


			//principal variation search
			if (score > alpha)
			{
				//test if the node can improve its alpha in the closed window
				score = -Search(max_depth, depth + 1, -alpha - 1, -alpha);

				//if the improved alpha does not exceed the beta in open window
				//then research this node, because it may be a new best move
				if (score > alpha && score < beta) score = -Search(max_depth, depth + 1, -beta, -alpha);
			}
		}

		RestoreState();

		if (score > alpha)
		{
			alpha = score;
			is_pv_node_candidate = true;
			hash_flag = Transposition::HASH_FLAG_EXACT;

			//update the current pv move and copy over from child's pv moves
			int next_depth = depth + 1;
			this->pv_length[depth] = this->pv_length[next_depth];
			this->pv_table[depth][depth] = pseudo_move;
			copy(this->pv_table[next_depth] + next_depth, this->pv_table[next_depth] + this->pv_length[next_depth], this->pv_table[depth] + next_depth);


			if (score >= beta)
			{
				//record the quiet killer moves that causes fail high
				if (pseudo_move.IsQuietMove())
				{
					killer_heuristic[depth][1] = killer_heuristic[depth][0];
					killer_heuristic[depth][0] = pseudo_move;
				}

				Transposition::RecordHash(this->transposition_table, TRANSPOSITION_TABLE_SIZE, { this->boardstate.position_key, depth_searched_beyond, beta, Transposition::HASH_FLAG_BETA }, this->boardstate.side_to_move);
				return beta;
			}
		}

		++legal_moves_searched;
	}

	if (legal_moves_searched == 0)
	{
		//include the depth to force the shorter checkmate
		if (in_check) alpha = MATE_SCORE + depth;
		else alpha = 0;

		hash_flag = Transposition::HASH_FLAG_EXACT;
	}

	Transposition::RecordHash(this->transposition_table, TRANSPOSITION_TABLE_SIZE, { this->boardstate.position_key, depth_searched_beyond, alpha, hash_flag }, this->boardstate.side_to_move);
	return alpha;
}



int Board::Quiescence(int alpha, int beta)
{
	//threefold repetition
	if (this->repeated_position[this->boardstate.position_key & (REPEATED_POSITION_SIZE - 1)]) return 0;

	int score = (this->neural_network_evaluation ? NeuralNetworkEvaluate() : Evaluate());

	if (score >= beta) return beta;
	alpha = max(alpha, score);

	vector<Move> pseudo_moves = GetPseudoMoves();
	SortNonQuietMoves(pseudo_moves);

	for (Move pseudo_move : pseudo_moves)
	{
		if (this->timer.IsTimeOut()) break;

		if (!pseudo_move.IsQuietMove())
		{
			if (MakePseudoMove(pseudo_move))
			{
				int score = -Quiescence(-beta, -alpha);
				RestoreState();

				if (score >= beta) return beta;
				alpha = max(alpha, score);
			}
		}
	}

	return alpha;
}




void Board::PrintBoard()
{
	vector<int> board(64, -1);

	for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
	{
		for (Bitboard bitboard = this->boardstate.bitboards[piece]; bitboard; bitboard.PopBit())
		{
			int square = bitboard.GetLeastSigBit();
			board[square] = piece;
		}
	}


	for (int rank = 0; rank < 8; ++rank)
	{
		cout << "  -------------------------------\n ";
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;

			cout << "| " << PieceToAscii(board[square]) << ' ';
		}
		cout << "| " << 8 - rank << '\n';
	}

	cout << "  -------------------------------\n   a   b   c   d   e   f   g   h\n";

	cout << "Enpassant: " << (this->boardstate.enpassant_square != INVALID_SQUARE ? SQUARE_STR_TABLE[this->boardstate.enpassant_square] : "empty") << '\n';
	cout << "Castle: " << this->boardstate.castle << '\n';
	cout << "Position Key: " << this->boardstate.position_key << '\n';
	if (this->boardstate.side_to_move == WHITE) cout << "White to move:\n";
	else cout << "Black to move:\n";
}


Board::Board() : boardstate(), boardstate_history(BOARDSTATE_STACK_SIZE), visited_nodes(0), pv_length{}, pv_table{}, killer_heuristic{}, repeated_position(new bool[REPEATED_POSITION_SIZE]()), transposition_table(new Transposition[TRANSPOSITION_TABLE_SIZE]()), model(MODEL_TOPOLOGY), neural_network_evaluation(false), timer()
{
	if (model.LoadNeuralNetwork(MODEL_FILE_NAME)) std::cout << "Neural Network file loaded\n";
	else std::cout << "Failed to load the Neural Network file\n" << MODEL_FILE_NAME << " is missing" << '\n';

	this->ParseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}