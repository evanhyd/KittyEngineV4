#pragma once
class Move
{
	/*
	            binary move bits           hexidecimal constants
         
         0000 0000 0000 0000 0011 1111    source square               0x3f
         0000 0000 0000 1111 1100 0000    dest square         >> 6  & 0x3f
         0000 0000 1111 0000 0000 0000    moved piece         >> 12 & 0xf
         0000 1111 0000 0000 0000 0000    promoted piece      >> 16 & 0xf
         0001 0000 0000 0000 0000 0000    capture flag           0x100000
         0010 0000 0000 0000 0000 0000    enpassant flag         0x200000
         0100 0000 0000 0000 0000 0000    double push flag       0x400000
         1000 0000 0000 0000 0000 0000    castling flag          0x800000
	*/

    int move;


public:

    int GetSource() const;
    int GetDest() const;
    int GetMovedPiece() const;
    int GetPromotedPieceType() const;
    bool IsCapture() const;
    bool IsEnpassant() const;
    bool IsDoublePush() const;
    bool IsCastling() const;
    bool IsQuietMove() const;
    bool IsMoveEmpty() const;
    void Clear();
    void PrintMove() const;

    operator int() const;
    operator int&();

    Move();
    Move(int new_move);
    Move(int source_square, int dest_square, int piece, int promoted_piece_type = 0, int flag = 0);

    static constexpr int GET_SQUARE_MASK = 0x3f;
    static constexpr int GET_PIECE = 0xf;
    static constexpr int CAPTURE_FLAG = 0x100000; 
    static constexpr int ENPASSANT_FLAG = 0x200000;
    static constexpr int DOUBLE_PUSH_FLAG = 0x400000;
    static constexpr int CASTLING_FLAG = 0x800000;
    static constexpr int VIOLENT_MOVE_FLAG = GET_PIECE << 16 | CAPTURE_FLAG | ENPASSANT_FLAG;


    //move ordering
    static constexpr int FUTILITY_MOVE_PRIORITY = 0;
    static constexpr int KILLER_MOVE = 99;
    static constexpr int ENPASSANT_PRIORITY = 105;
    static constexpr int PROMOTION_PRIORITY = 500;

    static constexpr int CAPTURE_PRIORITY_TABLE[12][12] = 
    {
        {0, 0, 0, 0, 0, 0,  105, 205, 305, 405, 505, 0},
        {0, 0, 0, 0, 0, 0,  104, 204, 304, 404, 504, 0},
        {0, 0, 0, 0, 0, 0,  103, 203, 303, 403, 503, 0},
        {0, 0, 0, 0, 0, 0,  102, 202, 302, 402, 502, 0},
        {0, 0, 0, 0, 0, 0,  101, 201, 301, 401, 501, 0},
        {0, 0, 0, 0, 0, 0,  100, 200, 300, 400, 500, 0},
        {105, 205, 305, 405, 505, 0, 0, 0, 0, 0, 0, 0,},
        {104, 204, 304, 404, 504, 0, 0, 0, 0, 0, 0, 0,},
        {103, 203, 303, 403, 503, 0, 0, 0, 0, 0, 0, 0,},
        {102, 202, 302, 402, 502, 0, 0, 0, 0, 0, 0, 0,},
        {101, 201, 301, 401, 501, 0, 0, 0, 0, 0, 0, 0,},
        {100, 200, 300, 400, 500, 0, 0, 0, 0, 0, 0, 0,}
    };

    static constexpr int PRINCIPAL_VARIATION_PRIORITY = 10000;

};

