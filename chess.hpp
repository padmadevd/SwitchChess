#ifndef CHESS_HPP
#define CHESS_HPP

#include <vector>
#include <string>

// SQUARE INDICES MAPPING
#define A8 0
#define B8 1
#define C8 2
#define D8 3
#define E8 4
#define F8 5
#define G8 6
#define H8 7

#define A7 8
#define B7 9
#define C7 10
#define D7 11
#define E7 12
#define F7 13
#define G7 14
#define H7 15

#define A6 16
#define B6 17
#define C6 18
#define D6 19
#define E6 20
#define F6 21
#define G6 22
#define H6 23

#define A5 24
#define B5 25
#define C5 26
#define D5 27
#define E5 28
#define F5 29
#define G5 30
#define H5 31

#define A4 32
#define B4 33
#define C4 34
#define D4 35
#define E4 36
#define F4 37
#define G4 38
#define H4 39

#define A3 40
#define B3 41
#define C3 42
#define D3 43
#define E3 44
#define F3 45
#define G3 46
#define H3 47

#define A2 48
#define B2 49
#define C2 50
#define D2 51
#define E2 52
#define F2 53
#define G2 54
#define H2 55

#define A1 56
#define B1 57
#define C1 58
#define D1 59
#define E1 60
#define F1 61
#define G1 62
#define H1 63

#define SQUARE_NONE 100

uint8_t North(uint8_t square);
uint8_t South(uint8_t square);
uint8_t East(uint8_t square);
uint8_t West(uint8_t square);

// PIECE - COLOR
#define COLOR_NONE 0
#define COLOR_B 1
#define COLOR_W 2
#define COLOR_ALL (COLOR_W|COLOR_B)

// PIECE - TYPE
#define BISHOP 0
#define KING 1
#define KNIGHT 2
#define PAWN 3
#define QUEEN 4
#define ROOK 5
#define PIECE_NONE 6

// PIECES
#define EMPTY 0
#define RB 1
#define NB 2
#define BB 3
#define QB 4
#define KB 5
#define PB 6
#define RW 7
#define NW 8
#define BW 9
#define QW 10
#define KW 11
#define PW 12
#define INVALID 13

uint8_t ColorOf(uint8_t piece);
uint8_t TypeOf(uint8_t piece);

inline bool IsRook(uint8_t piece)
{
	return (piece == RB || piece == RW);
}
inline bool IsKnight(uint8_t piece)
{
	return (piece == NB || piece == NW);
}
inline bool IsBishop(uint8_t piece)
{
	return (piece == BB || piece == BW);
}
inline bool IsQueen(uint8_t piece)
{
	return (piece == QB || piece == QW);
}
inline bool IsKing(uint8_t piece)
{
	return (piece == KB || piece == KW);
}
inline bool IsPawn(uint8_t piece)
{
	return (piece == PB || piece == PW);
}
inline bool IsBlack(uint8_t piece)
{
	return (piece >= RB && piece <= PB);
}
inline bool IsWhite(uint8_t piece)
{
	return (piece >= RW && piece <= PW);
}
inline bool IsEmpty(uint8_t piece)
{
	return (piece == EMPTY);
}
inline bool IsValid(uint8_t piece)
{
	return (piece >= 0 && piece <= PW);
}
inline bool IsValidSquare(uint8_t square)
{
	return (square < 64 && square >= 0);
}

// MOVE TYPES
#define NORMAL 0
#define TWOSTEP 1
#define CASTLING 2
#define ENPASSANT 3
#define PROMOTION 4

// GAME END TYPE
#define CHECKMATE 0
#define STALEMATE 1
#define DEAD_POSITION 2
#define FIFTY_MOVE 3

// MOVE STRUCTURE DEFINING A SINGLE MOVE
struct Move
{
	uint8_t _type = NORMAL;
	uint8_t _start = SQUARE_NONE;
	uint8_t _end = SQUARE_NONE;
	uint8_t _inserted = INVALID;
	uint8_t _deleted = INVALID;
};

struct Board
{
	uint8_t _squares[64];
	std::vector<Move> _move_history;
	uint32_t _move_count;
	std::vector<uint32_t> _half_move_history;
	int32_t _half_move_clock;
	int32_t _full_move_clock;
	uint8_t _current_color;

	uint8_t _kw_square;
	uint8_t _kb_square;

	uint32_t _uncastle_move_w;
	uint32_t _uncastle_move_w_q;
	uint32_t _uncastle_move_b;
	uint32_t _uncastle_move_b_q;

	bool _can_castle_w;
	bool _can_castle_w_q;
	bool _can_castle_b;
	bool _can_castle_b_q;

	uint8_t _game_end_type;

	Board();
	void Reset();

	uint8_t At(uint8_t square);
	uint8_t At(uint8_t x, uint8_t y);
	void Set(uint8_t square, uint8_t piece);
	uint8_t GetKing(uint8_t color);
	std::vector<uint8_t> GetPieceCount();

	void MakeMove(Move move);
	void UnMakeMove();

	bool IsInAttack(uint8_t square, uint8_t color);
	bool IsInCheck(uint8_t color);

	std::vector<Move> GetPseudoLegalMoves(uint8_t square);
	std::vector<Move> GetAllPseudoLegalMoves(uint8_t player);
	std::vector<Move> GetLegalMoves(uint8_t square);
	std::vector<Move> GetAllLegalMoves(uint8_t player);

	void SetPositionFromFENString(std::string fen);
	std::string GetFENString();
	Move GetMoveFromString(std::string move);
	void MakeMove(std::string move);

	bool IsGameFinished();
};

void DebugPieceType(uint8_t type);
void DebugPieceColor(uint8_t color);
void DebugPiece(uint8_t piece);
void DebugMoveType(uint8_t type);
void DebugMove(Move move);
void DebugSquare(uint8_t square);
void DebugBoard(Board &board);

#endif