#pragma once
class Move
{
	/*
	            binary move bits           hexidecimal constants
         
         0000 0000 0000 0000 0011 1111    source square               0x3f
         0000 0000 0000 1111 1100 0000    dest square         >> 6  & 0x3f
         0000 0000 1111 0000 0000 0000    piece               >> 12 & 0xf
         0000 1111 0000 0000 0000 0000    promoted piece      >> 16 & 0xf
         0001 0000 0000 0000 0000 0000    capture flag           0x100000
         0010 0000 0000 0000 0000 0000    double push flag       0x200000
         0100 0000 0000 0000 0000 0000    enpassant flag         0x400000
         1000 0000 0000 0000 0000 0000    castling flag          0x800000
	*/

    int move;
    //int score;


public:

    int GetSource();
    int GetDest();
    int GetPiece();
    int GetPromotedPiece();
    bool IsCapture();
    bool IsDoublePush();
    bool IsEnpassant();
    bool IsCastling();
    void PrintMove();

    Move() = default;
    Move(int source_square, int dest_square, int piece, int promoted_piece, int flag);


    static constexpr int CAPTURE_FLAG = 0x100000;
    static constexpr int DOUBLE_PUSH_FLAG = 0x200000;
    static constexpr int ENPASSANT_FLAG = 0x400000;
    static constexpr int CASTLING_FLAG = 0x800000;
};

