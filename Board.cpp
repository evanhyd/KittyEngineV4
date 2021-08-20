#include "board.h"
#include "piece_attack_table.h"
#include <iostream>
#include <cctype>
#include <vector>

using namespace std;


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
	}
}

int GetRank(int square)
{
	return square >> 3;
}
int GetFile(int square)
{
	return square & 7;
}



void Board::ParseFEN(const string& FEN)
{
	this->boardstate.Clear();
	this->boardstate_history.Clear();


	auto iter = FEN.begin();
	for (int square = 0; square < 64;)
	{
		char letter = *iter++;
		if (isdigit(letter)) square += letter - '0';
		else if (isalpha(letter)) this->boardstate.bitboard[FenToPiece(letter)].SetBit(square++);
	}

	for (int piece = WHITE_PAWN; piece <= WHITE_KING; ++piece)
	{
		this->boardstate.occupancies[WHITE] |= this->boardstate.bitboard[piece];
	}
	for (int piece = BLACK_PAWN; piece <= BLACK_KING; ++piece)
	{
		this->boardstate.occupancies[BLACK] |= this->boardstate.bitboard[piece];
	}
	this->boardstate.occupancies[BOTH] = this->boardstate.occupancies[WHITE] | this->boardstate.occupancies[BLACK];


	this->boardstate.side_to_move = (*++iter == 'w' ? WHITE : BLACK);

	iter += 2;

	this->boardstate.castle = 0;
	while (true)
	{
		if (*iter == ' ') break;

		switch (*iter)
		{
		case 'K': this->boardstate.castle |= WK; break;
		case 'Q': this->boardstate.castle |= WQ; break;
		case 'k': this->boardstate.castle |= BK; break;
		case 'q': this->boardstate.castle |= BQ; break;
		}
		++iter;
	}

	++iter;

	if (*iter != '-')
	{
		int file = *iter - 'a';
		++iter;
		int rank = 8 - (*iter - '0');
		this->boardstate.enpassant_square = rank * 8 + file;
	}
	else
	{
		this->boardstate.enpassant_square = INVALID_SQUARE;
	}

	iter += 2;
}

bool Board::ParseMove(const string& move_str)
{
	if (move_str.size() < 4) return false;

	int source_file = tolower(move_str[0]) - 'a';
	int source_rank = tolower(move_str[1]) - '0';
	int dest_file = tolower(move_str[2]) - 'a';
	int dest_rank = tolower(move_str[3]) - '0';
	int promoted_piece_type = 0;
	if (move_str.size() >= 5) promoted_piece_type = FenToPieceType(move_str[4]);

	int source_square = source_rank * 8 + source_file;
	int dest_square = dest_rank * 8 + dest_file;

	vector<Move> pseudo_moves = GenerateMoves();
	for (Move pseudo_move : pseudo_moves)
	{
		if (pseudo_move.GetSource() == source_square && pseudo_move.GetDest() == dest_square && pseudo_move.GetPromotedPieceType() == promoted_piece_type)
		{
			if (MakeMove(pseudo_move)) return true;
			else return false;
		}
	}

	return false;
}


void Board::ParsePosition(const std::string& position_str)
{
	size_t index = 9;

	if (position_str.substr(index, 8) == "startpos")
	{
		index += 9;
		ParseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	}
	else if (position_str.substr(index, 3) == "fen")
	{
		index += 4;
		ParseFEN(position_str.substr(index));
	}

	/*
	if (index < position_str.size())
	{
		//4 bytes assembly level KMP
		//index = strstr(position_str.c_str(), "move") - position_str.c_str();
		index = position_str.find("move");
		if (index != string::npos)
		{
			index += 5;
			while (true)
			{
				size_t space_index = position_str.find(" ");
			}
		}
	}*/
}



bool Board::IsSquareAttacked(int square, int attack_side)
{
	//pawn
	int pawn = PIECE_LIST_TABLE[attack_side][PAWN];
	if (GetPawnAttackExact(!attack_side, square) & this->boardstate.bitboard[pawn]) return true;

	int knight = PIECE_LIST_TABLE[attack_side][KNIGHT];
	if (GetKnightAttackExact(square) & this->boardstate.bitboard[knight]) return true;

	int bishop = PIECE_LIST_TABLE[attack_side][BISHOP];
	if (GetBishopAttackExact(square, this->boardstate.occupancies[BOTH]) & this->boardstate.bitboard[bishop]) return true;

	int rook = PIECE_LIST_TABLE[attack_side][ROOK];
	if (GetRookAttackExact(square, this->boardstate.occupancies[BOTH]) & this->boardstate.bitboard[rook]) return true;

	int queen = PIECE_LIST_TABLE[attack_side][QUEEN];
	if (GetQueenAttackExact(square, this->boardstate.occupancies[BOTH]) & this->boardstate.bitboard[queen]) return true;

	int king = PIECE_LIST_TABLE[attack_side][KING];
	if (GetKingAttackExact(square) & this->boardstate.bitboard[king]) return true;

	return false;
}


vector<Move> Board::GenerateMoves()
{
	vector<Move> pseudo_moves;
	pseudo_moves.reserve(218);

	int side = this->boardstate.side_to_move;
	int pawn = PIECE_LIST_TABLE[side][PAWN];

	//reverse the occupancy, where bit 1 represents an empty space
	Bitboard not_occupancy = ~this->boardstate.occupancies[BOTH];

	Bitboard pawn_push = (side == WHITE ? this->boardstate.bitboard[pawn] >> 8 : this->boardstate.bitboard[pawn] << 8) & not_occupancy;
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
	

	Bitboard double_pawn_push = (side == WHITE ? (pawn_push & GET_RANK_3_MASK) >> 8 : (pawn_push & GET_RANK_6_MASK) << 8) & not_occupancy;
	while (double_pawn_push)
	{
		int dest_square = double_pawn_push.GetLeastSigBit();
		int source_square = (side == WHITE ? dest_square + 16 : dest_square - 16);
		pseudo_moves.push_back(Move(source_square, dest_square, pawn, 0, Move::DOUBLE_PUSH_FLAG));
		double_pawn_push.PopBit();
	}


	for (Bitboard pawn_bitboard = this->boardstate.bitboard[pawn]; pawn_bitboard; pawn_bitboard.PopBit())
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
		for (Bitboard pawn_attack = PAWN_ATTACK_TABLE[!side][dest_square] & this->boardstate.bitboard[pawn]; pawn_attack; pawn_attack.PopBit())
		{
			int source_square = pawn_attack.GetLeastSigBit();

			//do i need capture flag?
			pseudo_moves.push_back(Move(source_square, dest_square, pawn, 0, Move::ENPASSANT_FLAG));
		}
	}









	int knight = PIECE_LIST_TABLE[side][KNIGHT];
	for (Bitboard knight_bitboard = this->boardstate.bitboard[knight]; knight_bitboard; knight_bitboard.PopBit())
	{
		int source_square = knight_bitboard.GetLeastSigBit();

		//knight attacks empty or enemy occupied squares
		for (Bitboard knight_attack = KNIGHT_ATTACK_TABLE[source_square] & ~this->boardstate.occupancies[side]; knight_attack; knight_attack.PopBit())
		{
			int dest_square = knight_attack.GetLeastSigBit();
			if(this->boardstate.occupancies[!side].GetBit(dest_square)) pseudo_moves.push_back(Move(source_square, dest_square, knight, 0, Move::CAPTURE_FLAG));
			else pseudo_moves.push_back(Move(source_square, dest_square, knight));
		}
	}






	int king = PIECE_LIST_TABLE[side][KING];
	int king_square = this->boardstate.bitboard[king].GetLeastSigBit();

	for (Bitboard king_attack = KING_ATTACK_TABLE[king_square] & ~this->boardstate.occupancies[side]; king_attack; king_attack.PopBit())
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
			if ((this->boardstate.occupancies[BOTH] & CASTLE_OCCUPANCY_MASK_TABLE[side][castle_type]) == 0)
			{
				//king and the adjacent squares can not be in checked
				int adj_square = (castle_type == KING_SIDE ? king_square + 1 : king_square - 1);
				int dest_square = (castle_type == KING_SIDE ? king_square + 2 : king_square - 2);

				if (!IsSquareAttacked(king_square, !side) && !IsSquareAttacked(adj_square, !side))
				{
					pseudo_moves.push_back(Move(king_square, dest_square, king, 0, Move::CASTLING_FLAG));
				}
			}
		}
	}






	int bishop = PIECE_LIST_TABLE[side][BISHOP];
	for (Bitboard bishop_bitboard = this->boardstate.bitboard[bishop]; bishop_bitboard; bishop_bitboard.PopBit())
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
	for (Bitboard rook_bitboard = this->boardstate.bitboard[rook]; rook_bitboard; rook_bitboard.PopBit())
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
	for (Bitboard queen_bitboard = this->boardstate.bitboard[queen]; queen_bitboard; queen_bitboard.PopBit())
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

bool Board::MakeMove(Move move)
{
	SaveState();

	int side = this->boardstate.side_to_move;

	int source_square = move.GetSource();
	int dest_square = move.GetDest();
	int moving_piece = move.GetPiece();

	this->boardstate.bitboard[moving_piece].ClearBit(source_square);
	this->boardstate.bitboard[moving_piece].SetBit(dest_square);

	if (move.IsCapture())
	{
		for (int captured_piece = PIECE_LIST_TABLE[!side][PAWN]; captured_piece <= PIECE_LIST_TABLE[!side][QUEEN]; ++captured_piece)
		{
			if (this->boardstate.bitboard[captured_piece].GetBit(dest_square))
			{
				this->boardstate.bitboard[captured_piece].ClearBit(dest_square);
				break;
			}
		}
	}

	int promoted_piece_type = move.GetPromotedPieceType();
	if (promoted_piece_type)
	{
		int promoted_piece = PIECE_LIST_TABLE[side][promoted_piece_type];
		this->boardstate.bitboard[moving_piece].ClearBit(dest_square);
		this->boardstate.bitboard[promoted_piece].SetBit(dest_square);
	}
	
	if (move.IsEnpassant())
	{
		int enpassanted_pawn = PIECE_LIST_TABLE[!side][PAWN];
		int enpassanted_pawn_square = (side == WHITE ? dest_square + 8 : dest_square - 8);

		this->boardstate.bitboard[enpassanted_pawn].ClearBit(enpassanted_pawn_square);
	}

	if (move.IsDoublePush()) this->boardstate.enpassant_square = (side == WHITE ? source_square - 8 : source_square + 8);
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
		this->boardstate.bitboard[rook].ClearBit(rook_source);
		this->boardstate.bitboard[rook].SetBit(rook_dest);
	}

	this->boardstate.castle &= CASTLING_PERMISSION_FILTER_TABLE[source_square];
	this->boardstate.castle &= CASTLING_PERMISSION_FILTER_TABLE[dest_square];



	//update the occupancies
	this->boardstate.occupancies[WHITE] = 0;
	this->boardstate.occupancies[BLACK] = 0;
	for (int piece = WHITE_PAWN; piece <= WHITE_KING; ++piece)
	{
		this->boardstate.occupancies[WHITE] |= this->boardstate.bitboard[piece];
	}
	for (int piece = BLACK_PAWN; piece <= BLACK_KING; ++piece)
	{
		this->boardstate.occupancies[BLACK] |= this->boardstate.bitboard[piece];
	}
	this->boardstate.occupancies[BOTH] = this->boardstate.occupancies[WHITE] | this->boardstate.occupancies[BLACK];



	//check for check
	int king = PIECE_LIST_TABLE[side][KING];
	int king_square = this->boardstate.bitboard[king].GetLeastSigBit();

	this->boardstate.side_to_move = !this->boardstate.side_to_move;
	if (IsSquareAttacked(king_square, this->boardstate.side_to_move))
	{
		RestoreState();
		return false;
	}
	return true;
}

void Board::SaveState()
{
	this->boardstate_history.PushBack(this->boardstate);
}
void Board::RestoreState()
{
	this->boardstate = this->boardstate_history.Back();
	this->boardstate_history.PopBack();
}



void Board::PerfTest(int depth, int& node_visited)
{
	if (depth == 0)
	{
		++node_visited;
		return;
	}

	vector<Move> pseudo_moves = GenerateMoves();

	for (Move move : pseudo_moves)
	{
		if (MakeMove(move))
		{
			PerfTest(depth - 1, node_visited);
			RestoreState();
		}
	}

	return;
}


void Board::PrintBoard()
{
	for (int rank = 0; rank < 8; ++rank)
	{
		cout << "  -------------------------------\n ";
		for (int file = 0; file < 8; ++file)
		{
			int square = rank * 8 + file;

			bool found = false;
			for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece)
			{
				if (this->boardstate.bitboard[piece].GetBit(square))
				{
					cout << "| " << PieceToFen(piece) << ' ';
					found = true;
					break;
				}
			}

			if (!found) cout << "|   ";
		}
		cout << "| " << 8 - rank << '\n';
	}

	cout << "  -------------------------------\n   a   b   c   d   e   f   g   h\n";

	cout << "Castle: " << this->boardstate.castle << '\n';
	cout << "Enpassant: " << (this->boardstate.enpassant_square != INVALID_SQUARE ? SQUARE_STR_TABLE[this->boardstate.enpassant_square] : "empty") << '\n';

	if (this->boardstate.side_to_move == WHITE) cout << "White to move:\n";
	else cout << "Black to move:\n";
}



void Board::GUI()
{

}


Board::Board(const string& FEN) : boardstate_history(256)
{
	this->ParseFEN(FEN);
}