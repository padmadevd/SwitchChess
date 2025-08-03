#include <chess.hpp>

#include <stdio.h>
#include <sstream>

void DebugPieceType(uint8_t type){

	switch (type) {
		case PAWN:
			printf("PAWN ");
			break;
		case KNIGHT:
			printf("KNIGHT ");
			break;
		case ROOK:
			printf("ROOK ");
			break;
		case BISHOP:
			printf("BISHOP ");
			break;
		case QUEEN:
			printf("QUEEN ");
			break;
		case KING:
			printf("KING ");
			break;
		case PIECE_NONE:
			printf("PIECE NONE ");
			break;
	}

	fflush(stdout);
	return;
}
void DebugPieceColor(uint8_t color){

	switch (color) {
		case COLOR_B:
			printf("BLACK ");
			break;
		case COLOR_W:
			printf("WHITE ");
			break;
		case COLOR_ALL:
			printf("COLOR ALL ");
			break;
		case COLOR_NONE:
			printf("COLOR NONE ");
			break;
	}

	fflush(stdout);
	return;
}
void DebugPiece(uint8_t piece){

	DebugPieceType(TypeOf(piece));
	DebugPieceColor(ColorOf(piece));

	fflush(stdout);
	return;
}
void DebugMoveType(uint8_t type){

	switch (type) {
		case NORMAL:
			printf("MOVE-NORMAL ");
			break;
		case TWOSTEP:
			printf("MOVE-TWOSTEP ");
			break;
		case CASTLING:
			printf("MOVE-CASTLING ");
			break;
		case ENPASSANT:
			printf("MOVE-ENPASSANT ");
			break;
		case PROMOTION:
			printf("MOVE-PROMOTION ");
			break;
	}

	fflush(stdout);
	return;
}

void DebugMove(Move move){

	DebugMoveType(move._type);
	printf("FROM ");
	DebugSquare(move._start);
	printf("END ");
	DebugSquare(move._end);
	printf("INSERTED ");
	DebugPiece(move._inserted);
	printf("DELETED ");
	DebugPiece(move._deleted);

	fflush(stdout);
	return;
}
void DebugSquare(uint8_t square){

	if(IsValidSquare(square)){

		uint8_t x = square%8;
		uint8_t y = square/8;
		printf("(%c%c) ", x+'a', '8'-y);
	}else {
		printf("SQUARE-NONE ");
	}

	fflush(stdout);
	return;
}

void DebugBoard(Board &board){

	for(int y = 0; y < 8; y++){
		for (int x = 0; x < 8; ++x)
		{
			printf("%d ", board.At(x, y));
		}
		printf("\n");
	}

	fflush(stdout);
}

uint8_t North(uint8_t square)
{
	if(square != SQUARE_NONE && square/8 > 0) return square - 8;
	return SQUARE_NONE;
}
uint8_t South(uint8_t square)
{
	if(square != SQUARE_NONE && square/8 < 7) return square + 8;
	return SQUARE_NONE;
}
uint8_t East(uint8_t square)
{
	if(square != SQUARE_NONE && square%8 < 7) return square + 1;
	return SQUARE_NONE;
}
uint8_t West(uint8_t square)
{
	if(square != SQUARE_NONE && square%8 > 0) return square - 1;
	return SQUARE_NONE;
}

uint8_t ColorOf(uint8_t piece)
{
	if(piece >= RW && piece <= PW) return COLOR_W;
	else if(piece >= RB && piece <= PB) return COLOR_B;
	return COLOR_NONE;
}

uint8_t TypeOf(uint8_t piece)
{
	if(piece == RW || piece == RB) return ROOK;
	else if(piece == NW || piece == NB) return KNIGHT;
	else if(piece == BW || piece == BB) return BISHOP;
	else if(piece == QW || piece == QB) return QUEEN;
	else if(piece == KW || piece == KB) return KING;
	else if(piece == PW || piece == PB) return PAWN;
	return PIECE_NONE;
}

Board::Board()
{
	Reset();
}

void Board::Reset()
{
	uint8_t positions[64] = {
		 RB, NB, BB, QB, KB, BB, NB, RB,
		 PB, PB, PB, PB, PB, PB, PB, PB,
		  0,  0,  0,  0,  0,  0,  0,  0,
		  0,  0,  0,  0,  0,  0,  0,  0,
		  0,  0,  0,  0,  0,  0,  0,  0,
		  0,  0,  0,  0,  0,  0,  0,  0,
		 PW, PW, PW, PW, PW, PW, PW, PW,
		 RW, NW, BW, QW, KW, BW, NW, RW
	};

	for(uint8_t i = 0; i < 64; ++i)
	{
		_squares[i] = positions[i];
		if(_squares[i] == KW) _kw_square = i;
		if(_squares[i] == KB) _kb_square = i;
	}

	_uncastle_move_w = -1;
	_uncastle_move_w_q = -1;
	_uncastle_move_b = -1;
	_uncastle_move_b_q = -1;

	_can_castle_w = true;
	_can_castle_w_q = true;
	_can_castle_b = true;
	_can_castle_b_q = true;

	_move_history.clear();
	_move_count = 0;
	_half_move_clock = 0;
	_full_move_clock = 1;
	_current_color = COLOR_W;
}

uint8_t Board::At(uint8_t square)
{
	if(IsValidSquare(square)) return _squares[square];
	return INVALID;
}

uint8_t Board::At(uint8_t x, uint8_t y)
{
	if(x < 0 || x > 7 || y < 0 || y > 7) return INVALID;
	return _squares[y*8+x];
}

void Board::Set(uint8_t square, uint8_t piece)
{
	if (!IsValidSquare(square) || !IsValid(piece)) return;

	if(piece == KB) _kb_square = square;
	if(piece == KW) _kw_square = square;
	_squares[square] = piece;
}

uint8_t Board::GetKing(uint8_t color)
{
	if(color != COLOR_B && color != COLOR_W) return SQUARE_NONE;

	if(color == COLOR_B) return _kb_square;
	if(color == COLOR_W) return _kw_square;
	return SQUARE_NONE;
}

bool Board::IsInAttack(uint8_t square, uint8_t color)
{
	if(!IsValidSquare(square)) return false;
	
	uint8_t piece = At(square);
	if(!IsValid(piece)) return false;

	if(piece != EMPTY) color = ColorOf(piece);
	uint8_t d, s;

	// EAST
	s = East(square);
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == ROOK || type == QUEEN || (type == KING && d == 1)) return true;
				else break;
			}
		}
		s = East(s);
		d += 1;
	}

	// WEST
	s = West(square);
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else{
				uint8_t type = TypeOf(At(s));
				if(type == ROOK || type == QUEEN || (type == KING && d == 1)) return true;
				else break;
			}
		}
		s = West(s);
		d += 1;
	}
	
	// SOUTH
	s = South(square);
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == ROOK || type == QUEEN || (type == KING && d == 1)) return true;
				else break;
			}
		}
		s = South(s);
		d += 1;
	}

	// NORTH
	s = North(square);
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == ROOK || type == QUEEN || (type == KING && d == 1)) return true;
				else break;
			}
		}
		s = North(s);
		d += 1;
	}

	// NORTH - EAST
	s = East(North(square));
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == BISHOP || type == QUEEN || (type == KING && d == 1) || (IsBlack(At(s)) && type == PAWN && d == 1)) return true;
				else break;
			}
		}
		s = East(North(s));
		d += 1;
	}

	// NORTH - WEST
	s = West(North(square));
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == BISHOP || type == QUEEN || (type == KING && d == 1) || (IsBlack(At(s)) && type == PAWN && d == 1)) return true;
				else break;
			}
		}
		s = West(North(s));
		d += 1;
	}

	//SOUTH - EAST
	s = East(South(square));
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == BISHOP || type == QUEEN || (type == KING && d == 1) || (IsWhite(At(s)) && type == PAWN && d == 1)) return true;
				else break;
			}
		}
		s = East(South(s));
		d += 1;
	}

	//SOUTH - WEST
	s = West(South(square));
	d = 1;
	while(IsValidSquare(s))
	{
		if(!IsEmpty(At(s)))
		{
			if(ColorOf(At(s)) == color) break;
			else
			{
				uint8_t type = TypeOf(At(s));
				if(type == BISHOP || type == QUEEN || (type == KING && d == 1) || (IsWhite(At(s)) && type == PAWN && d == 1)) return true;
				else break;
			}
		}
		s = West(South(s));
		d += 1;
	}

	int xL[] = {2, 2, -2, -2, 1, 1, -1, -1};
	int yL[] = {1, -1, 1, -1, 2, -2, 2, -2};
	for(uint8_t i = 0; i < 8; ++i)
	{
		uint8_t x = square%8 + xL[i];
		uint8_t y = square/8 + yL[i];
		if((y >= 0 && y < 8) && (x >= 0 && x < 8) && TypeOf(At(x, y)) == KNIGHT && ColorOf(At(x, y)) != color) return true;
	}

	return false;
}

bool Board::IsInCheck(uint8_t color)
{
	return IsInAttack(GetKing(color), color);
}

#include <iostream>

void Board::MakeMove(Move move)
{
	if(!IsValidSquare(move._start) || !IsValidSquare(move._end) || move._start == move._end)
		return;

	_move_count += 1;
	if(_can_castle_b && (At(move._start) == KB || move._start == H8 || move._end == H8))
	{
		_can_castle_b = false;
		_uncastle_move_b = _move_count;
	}
	if(_can_castle_b_q && (At(move._start) == KB || move._start == A8 || move._end == A8))
	{
		_can_castle_b_q = false;
		_uncastle_move_b_q = _move_count;
	}
	if(_can_castle_w && (At(move._start) == KW || move._start == H1 || move._end == H1))
	{
		_can_castle_w = false;
		_uncastle_move_w = _move_count;
	}
	if(_can_castle_w_q && (At(move._start) == KW || move._start == A1 || move._end == A1))
	{
		_can_castle_w_q = false;
		_uncastle_move_w_q = _move_count;
	}

	if(move._type == NORMAL || move._type == TWOSTEP)
	{
		move._deleted = At(move._end);
		if(At(move._start) == KB) _kb_square = move._end;
		if(At(move._start) == KW) _kw_square = move._end;
		Set(move._end, At(move._start));
		Set(move._start, EMPTY);
	}
	else if(move._type == CASTLING)
	{
		move._deleted = EMPTY;
		if(At(move._start) == KB) _kb_square = move._end;
		if(At(move._start) == KW) _kw_square = move._end;
		Set(move._end, At(move._start));
		Set(move._start, EMPTY);
		if(move._end == G8 || move._end == G1)
		{
			Set(East(move._start), At(East(move._end)));
			Set(East(move._end), EMPTY);
		}
		else
		{
			Set(West(move._start), At(West(West(move._end))));
			Set(West(West(move._end)), EMPTY);
		}
	}
	else if(move._type == ENPASSANT)
	{
		if(IsWhite(At(move._start)))
		{
			Set(move._end, At(move._start));
			Set(move._start, EMPTY);
			move._deleted = At(South(move._end));
			Set(South(move._end), EMPTY);
		}
		else
		{
			Set(move._end, At(move._start));
			Set(move._start, EMPTY);
			move._deleted = At(North(move._end));
			Set(North(move._end), EMPTY);
		}
	}
	else if(move._type == PROMOTION)
	{
		move._deleted = At(move._end);
		Set(move._end, move._inserted);
		Set(move._start, EMPTY);
	}
	
	_move_history.push_back(move);

	if((move._deleted != INVALID && move._deleted != EMPTY) || TypeOf(At(move._end)) == PAWN)
	{
		_half_move_history.push_back(_half_move_clock);
		_half_move_clock = -1;
	}

	_half_move_clock++;
	if(_current_color == COLOR_B) _full_move_clock++;

	if(_current_color == COLOR_W) _current_color = COLOR_B;
	else _current_color = COLOR_W;
}

void Board::UnMakeMove()
{
	if(_move_history.size() <= 0)
		return;
	
	Move &move = _move_history.back();
	if(!IsValidSquare(move._start) || !IsValidSquare(move._end)) return;

	if(!_can_castle_b && _move_count == _uncastle_move_b) _can_castle_b = true;
	if(!_can_castle_w && _move_count == _uncastle_move_w) _can_castle_w = true;
	if(!_can_castle_b_q && _move_count == _uncastle_move_b_q) _can_castle_b_q = true;
	if(!_can_castle_w_q && _move_count == _uncastle_move_w_q) _can_castle_w_q = true;

	if(move._type == NORMAL || move._type == TWOSTEP)
	{
		if(At(move._end) == KB) _kb_square = move._start;
		if(At(move._end) == KW) _kw_square = move._start;
		Set(move._start, At(move._end));
		Set(move._end, move._deleted);
	}
	else if(move._type == CASTLING)
	{
		if(At(move._end) == KB) _kb_square = move._start;
		if(At(move._end) == KW) _kw_square = move._start;
		Set(move._start, At(move._end));
		Set(move._end, EMPTY);

		if(move._end == G8 || move._end == G1)
		{
			Set(East(move._end), At(East(move._start)));
			Set(East(move._start), EMPTY);
			if(IsBlack(At(move._start))) _can_castle_b = true;
			else _can_castle_w = true;
		}
		else
		{
			Set(West(West(move._end)), At(West(move._start)));
			Set(West(move._start), EMPTY);
			if(IsBlack(At(move._start))) _can_castle_b_q = true;
			else _can_castle_w_q = true;
		}
	}
	else if(move._type == ENPASSANT)
	{
		Set(move._start, At(move._end));
		if(IsWhite(At(move._end))) Set(South(move._end), move._deleted);
		else Set(North(move._end), move._deleted);
		Set(move._end, EMPTY);
	}
	else if(move._type == PROMOTION)
	{
		if(IsBlack(move._inserted))
		{
			Set(move._start, PB);
			Set(move._end, move._deleted);
		}
		else
		{
			Set(move._start, PW);
			Set(move._end, move._deleted);
		}
	}

	_move_history.pop_back();
	_move_count -= 1;

	if(_current_color == COLOR_W) _full_move_clock--;

	_half_move_clock--;
	if(_half_move_clock < 0 && !_half_move_history.empty())
	{
		_half_move_clock = _half_move_history.back();
		_half_move_history.pop_back();
	}

	if(_current_color == COLOR_W) _current_color = COLOR_B;
	else _current_color = COLOR_W;
}

std::vector<Move> Board::GetPseudoLegalMoves(uint8_t s)
{

	std::vector<Move> moves;

	if(s == SQUARE_NONE) return moves;

	uint8_t piece = At(s);
	uint8_t type = TypeOf(piece);
	uint8_t color = ColorOf(piece);

	uint8_t x, y, d;

	if(type == PAWN)
	{
		x = s%8;
		y = s/8;
		//PROMOTION MOVE
		if((color == COLOR_W && y == 1) || (color == COLOR_B && y == 6))
		{
			Move move;
			move._start = s;
			if(color == COLOR_W)
			{
				move._end = North(East(s));
				if (IsValidSquare(move._end) && IsBlack(At(move._end)))
				{
					move._deleted = At(move._end);
					move._type = PROMOTION;
					move._inserted = BW;
					moves.push_back(move);
					move._inserted = QW;
					moves.push_back(move);
					move._inserted = RW;
					moves.push_back(move);
					move._inserted = NW;
					moves.push_back(move);
				}
				move._end = North(West(s));
				if (IsValidSquare(move._end) && IsBlack(At(move._end)))
				{
					move._deleted = At(move._end);
					move._type = PROMOTION;
					move._inserted = BW;
					moves.push_back(move);
					move._inserted = QW;
					moves.push_back(move);
					move._inserted = RW;
					moves.push_back(move);
					move._inserted = NW;
					moves.push_back(move);
				}
				move._end = North(s);
				if (IsValidSquare(move._end) && At(move._end) == EMPTY)
				{
					move._deleted = EMPTY;
					move._type = PROMOTION;
					move._inserted = BW;
					moves.push_back(move);
					move._inserted = QW;
					moves.push_back(move);
					move._inserted = RW;
					moves.push_back(move);
					move._inserted = NW;
					moves.push_back(move);
				}
				
				return moves;
			}
			else
			{
				move._end = South(East(s));
				if(IsValidSquare(move._end) && IsWhite(At(move._end)))
				{
					move._deleted = At(move._end);
					move._type = PROMOTION;
					move._inserted = BB;
					moves.push_back(move);
					move._inserted = QB;
					moves.push_back(move);
					move._inserted = RB;
					moves.push_back(move);
					move._inserted = NB;
					moves.push_back(move);
				}
				move._end = South(West(s));
				if(IsValidSquare(move._end) && IsWhite(At(move._end)))
				{
					move._deleted = At(move._end);
					move._type = PROMOTION;
					move._inserted = BB;
					moves.push_back(move);
					move._inserted = QB;
					moves.push_back(move);
					move._inserted = RB;
					moves.push_back(move);
					move._inserted = NB;
					moves.push_back(move);
				}
				move._end = South(s);
				if(IsValidSquare(move._end) && At(move._end) == EMPTY)
				{
					move._deleted = At(move._end);
					move._type = PROMOTION;
					move._inserted = BB;
					moves.push_back(move);
					move._inserted = QB;
					moves.push_back(move);
					move._inserted = RB;
					moves.push_back(move);
					move._inserted = NB;
					moves.push_back(move);
				}
				return moves;
			}
		}
		//TWO STEP W
		if(color == COLOR_W && y == 6 && At(North(North(s))) == EMPTY && At(North(s)) == EMPTY)
		{
			Move move;
			move._start = s;
			move._end = North(North(s));
			move._type = TWOSTEP;
			moves.push_back(move);
		}
		//TWO STEP B
		if(color == COLOR_B && y == 1 && At(South(South(s))) == EMPTY && At(South(s)) == EMPTY)
		{
			Move move;
			move._start = s;
			move._end = South(South(s));
			move._type = TWOSTEP;
			moves.push_back(move);
		}
		//EN PASSANT
		if(_move_history.size() > 0)
		{
			Move prev_move = _move_history.back();
			if(prev_move._type == TWOSTEP && ColorOf(At(prev_move._end)) != color)
			{
				if(West(s) == prev_move._end || East(s) == prev_move._end)
				{
					if(color == COLOR_B)
					{
						Move move;
						move._start = s;
						move._end = South(prev_move._end);
						move._deleted = At(prev_move._end);
						move._type = ENPASSANT;
						moves.push_back(move);
					}
					else
					{
						Move move;
						move._start = s;
						move._end = North(prev_move._end);
						move._deleted = At(prev_move._end);
						move._type = ENPASSANT;
						moves.push_back(move);
					}
				}
			}
		}
	}

	// L - MOVES
	if(type == KNIGHT)
	{
		int xL[] = {2, 2, -2, -2, 1, 1, -1, -1};
		int yL[] = {1, -1, 1, -1, 2, -2, 2, -2};
		for(uint8_t i = 0; i < 8; ++i)
		{
			x = s%8 + xL[i];
			y = s/8 + yL[i];
			if((y >= 0 && y < 8) && (x >= 0 && x < 8) && ColorOf(At(x, y)) != color)
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
			}
		}
		return moves;
	}

	if(type == KING)
	{
		if((color == COLOR_B && _can_castle_b) || (color == COLOR_W && _can_castle_w))
		{
			if(!IsInAttack(s, color) && At(East(s)) == EMPTY && !IsInAttack(East(s), color) && At(East(East(s))) == EMPTY && !IsInAttack(East(East(s)), color))
			{
				Move move;
				move._start = s;
				move._end = East(East(s));
				move._type = CASTLING;
				moves.push_back(move);
			}
		}
		if((color == COLOR_B && _can_castle_b_q) || (color == COLOR_W && _can_castle_w_q))
		{
			if(!IsInAttack(s, color)  && At(West(s)) == EMPTY && !IsInAttack(West(s), color) && At(West(West(s))) == EMPTY && !IsInAttack(West(West(s)), color) && At(West(West(West(s)))) == EMPTY)
			{
				Move move;
				move._start = s;
				move._end = West(West(s));
				move._type = CASTLING;
				moves.push_back(move);
			}
		}
	}

	// EAST
	x = s%8 + 1;
	y = s/8;
	d = 1;
	while((type == ROOK || type == QUEEN || (type == KING && d == 1)) && (x >= 0 && x < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			Move move;
			move._start = s;
			move._end = y*8+x;
			move._type = NORMAL;
			moves.push_back(move);
		}
		x += 1;
		d += 1;
	}

	// WEST
	x = s%8 - 1;
	y = s/8;
	d = 1;
	while((type == ROOK || type == QUEEN || (type == KING && d == 1)) && (x >= 0 && x < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			Move move;
			move._start = s;
			move._end = y*8+x;
			move._type = NORMAL;
			moves.push_back(move);
		}
		x -= 1;
		d += 1;
	}

	// SOUTH
	x = s%8;
	y = s/8 + 1;
	d = 1;
	while((type == ROOK || type == QUEEN || (type == KING && d == 1) || (type == PAWN && d == 1 && color == COLOR_B)) && (y >= 0 && y < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color || type == PAWN) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			Move move;
			move._start = s;
			move._end = y*8+x;
			move._type = NORMAL;
			moves.push_back(move);
		}
		y += 1;
		d += 1;
	}

	// NORTH
	x = s%8;
	y = s/8 - 1;
	d = 1;
	while((type == ROOK || type == QUEEN || (type == KING && d == 1) || (type == PAWN && d == 1 && color == COLOR_W)) && (y >= 0 && y < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color || type == PAWN) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			Move move;
			move._start = s;
			move._end = y*8+x;
			move._type = NORMAL;
			moves.push_back(move);
		}
		y -= 1;
		d += 1;
	}

	// NORTH-EAST
	x = s%8 + 1;
	y = s/8 - 1;
	d = 1;
	while((type == BISHOP || type == QUEEN || (type == KING && d == 1) || (type == PAWN && d == 1 && color == COLOR_W)) && (x >= 0 && x < 8) && (y >= 0 && y < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			if(type != PAWN)
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
			}
		}
		x += 1;
		y -= 1;
		d += 1;
	}

	// NORTH-WEST
	x = s%8 - 1;
	y = s/8 - 1;
	d = 1;
	while((type == BISHOP || type == QUEEN || (type == KING && d == 1) || (type == PAWN && d == 1 && color == COLOR_W)) && (x >= 0 && x < 8) && (y >= 0 && y < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			if(type != PAWN)
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
			}
		}
		x -= 1;
		y -= 1;
		d += 1;
	}

	// SOUTH-EAST
	x = s%8 + 1;
	y = s/8 + 1;
	d = 1;
	while((type == BISHOP || type == QUEEN || (type == KING && d == 1) || (type == PAWN && d == 1 && color == COLOR_B)) && (x >= 0 && x < 8) && (y >= 0 && y < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			if(type != PAWN)
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
			}
		}
		x += 1;
		y += 1;
		d += 1;
	}

	// SOUTH-WEST
	x = s%8 - 1;
	y = s/8 + 1;
	d = 1;
	while((type == BISHOP || type == QUEEN || (type == KING && d == 1) || (type == PAWN && d == 1 && color == COLOR_B)) && (x >= 0 && x < 8) && (y >= 0 && y < 8))
	{
		if(At(x, y) != EMPTY)
		{
			if(ColorOf(At(x, y)) == color) break;
			else
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
				break;
			}
		}
		else
		{
			if(type != PAWN)
			{
				Move move;
				move._start = s;
				move._end = y*8+x;
				move._type = NORMAL;
				moves.push_back(move);
			}
		}
		x -= 1;
		y += 1;
		d += 1;
	}

	return moves;
}

std::vector<Move> Board::GetAllPseudoLegalMoves(uint8_t playerColor)
{
	std::vector<Move> moves;

	if(playerColor != COLOR_B && playerColor != COLOR_W) return moves;

	for(uint8_t s = 0; s < 64; s++)
	{
		uint8_t piece = At(uint8_t(s));
		uint8_t color = ColorOf(piece);
		if(color == playerColor)
		{
			std::vector<Move> pseudoMoves = GetPseudoLegalMoves(uint8_t(s));
			moves.insert(moves.end(), pseudoMoves.begin(), pseudoMoves.end());
		}
	}
	return moves;
}

std::vector<Move> Board::GetLegalMoves(uint8_t s)
{
	uint8_t piece = At(s);
	uint8_t color = ColorOf(piece);

	std::vector<Move> pseudoMoves = GetPseudoLegalMoves(s);
	std::vector<Move> moves;

	for(int i = 0; i < pseudoMoves.size(); i++)
	{
		Move m = pseudoMoves[i];
		MakeMove(m);
		if(!IsInCheck(color)) moves.push_back(m);
		UnMakeMove();
	}
	return moves;
}

std::vector<Move> Board::GetAllLegalMoves(uint8_t playerColor)
{
	std::vector<Move> moves;

	if(playerColor == COLOR_NONE || playerColor == COLOR_ALL) return moves;

	for(uint8_t s = 0; s < 64; s++)
	{
		uint8_t piece = At(uint8_t(s));
		uint8_t color = ColorOf(piece);
		if(color == playerColor)
		{
			std::vector<Move> m = GetLegalMoves(uint8_t(s));
			moves.insert(moves.end(), m.begin(), m.end());
		}
	}
	return moves;
}

std::string Board::GetFENString()
{
	std::string fen;
	for(int y = 0; y < 8; y++)
	{
		int empty = 0;
		int x = 0;
		while(x < 8)
		{
			if(At(x, y) == EMPTY) empty++;
			else
			{
				if(empty > 0) fen += std::to_string(empty);
				switch(At(x, y))
				{
					case RB:
						fen += 'r';
						break;
					case NB:
						fen += 'n';
						break;
					case BB:
						fen += 'b';
						break;
					case QB:
						fen += 'q';
						break;
					case KB:
						fen += 'k';
						break;
					case PB:
						fen += 'p';
						break;
					case RW:
						fen += 'R';
						break;
					case NW:
						fen += 'N';
						break;
					case BW:
						fen += 'B';
						break;
					case QW:
						fen += 'Q';
						break;
					case KW:
						fen += 'K';
						break;
					case PW:
						fen += 'P';
						break;
				}
				empty = 0;
			}
			x++;
		}
		if(empty > 0) fen += std::to_string(empty);
		if(y != 7) fen += '/';
	}
	fen += ' ';

	if(_current_color == COLOR_W) fen += 'w';
	else fen += 'b';
	fen += ' ';

	if(_can_castle_w) fen += 'K';
	if(_can_castle_w_q) fen += 'Q';
	if(!_can_castle_w && !_can_castle_w_q) fen += '-';
	if(_can_castle_b) fen += 'k';
	if(_can_castle_b_q) fen += 'q';
	if(!_can_castle_b && !_can_castle_b_q) fen += '-';
	fen += ' ';

	if(_move_history.size() > 0 && _move_history.back()._type == TWOSTEP)
	{
		Move move = _move_history.back();
		int s;
		if(ColorOf(At(move._end)) == COLOR_W) s = South(move._end);
		else s = North(move._end);

		fen += char('a'+s%8);
		fen += char('0'+(8-s/8));
	}
	else fen += '-';
	fen += ' ';

	fen += std::to_string(_half_move_clock);
	fen += ' ';

	fen += std::to_string(_full_move_clock);

	return fen;
}

void Board::SetPositionFromFENString(std::string fen)
{
	_can_castle_w = false;
	_can_castle_w_q = false;
	_can_castle_b = false;
	_can_castle_b_q = false;
	_move_count = 0;
	_move_history.clear();

	std::stringstream ss(fen);
	std::string word;

	ss>>word;
	int i = 0;
	int s = 0;
	while(i < word.size() && s < 64)
	{
		if(word[i] >= 'a' && word[i] <= 'z')
		{	
			switch(word[i])
			{
				case 'p':
					_squares[s] = PB;
					break;
				case 'b':
					_squares[s] = BB;
					break;
				case 'r':
					_squares[s] = RB;
					break;
				case 'q':
					_squares[s] = QB;
					break;
				case 'k':
					_squares[s] = KB;
					_kb_square = s;
					break;
				case 'n':
					_squares[s] = NB;
					break;
			}
			s++;
		}
		else if(word[i] >= 'A' && word[i] <= 'Z')
		{
			switch(word[i])
			{
				case 'P':
					_squares[s] = PW;
					break;
				case 'B':
					_squares[s] = BW;
					break;
				case 'R':
					_squares[s] = RW;
					break;
				case 'Q':
					_squares[s] = QW;
					break;
				case 'K':
					_squares[s] = KW;
					_kw_square = s;
					break;
				case 'N':
					_squares[s] = NW;
					break;
			}
			s++;
		}
		else
		{
			int j = s+word[i]-'0';
			while(s < j)
			{
				_squares[s] = EMPTY;
				s++;
			}
		}
		i++;
	}

	ss>>word;
	if(word[0] == 'b') _current_color = COLOR_B;
	else _current_color = COLOR_W;

	ss>>word;
	i = 0;
	while(i < word.size())
	{
		switch(word[i])
		{
			case 'K':
				_can_castle_w = true;
				break;
			case 'Q':
				_can_castle_w_q = true;
				break;
			case 'k':
				_can_castle_b = true;
				break;
			case 'q':
				_can_castle_b_q = true;
				break;
		}
		i++;
	}

	ss>>word;
	if(word[0] == '-') i++;
	else
	{
		s = ('8'-word[1])*8+(word[0]-'a');
		Move move;
		move._type = TWOSTEP;
		if(_current_color == COLOR_W)
		{
			move._start = North(s);
			move._end = South(s);
		}
		else
		{
			move._start = South(s);
			move._end = North(s);
		}
		_move_history.push_back(move);
		_move_count = 1;
	}

	ss>>word;
	_half_move_clock = std::stoi(word);

	ss>>word;
	_full_move_clock = std::stoi(word);
}

Move Board::GetMoveFromString(std::string move)
{
	if(move[0] >= 'a' && move[0] <= 'z') move[0] = 'A'+move[0]-'a';
	if(move[2] >= 'a' && move[2] <= 'z') move[2] = 'A'+move[2]-'a';

	int start = ('8'-move[1])*8+(move[0]-'A');
	int end = ('8'-move[3])*8+(move[2]-'A');

	std::vector<Move> moves = GetLegalMoves(start);
	for(Move m : moves)
	{
		if(m._end == end)
		{
			if(m._type == PROMOTION)
			{
				if(move[4] == 'q' && TypeOf(m._inserted) == QUEEN)
					return m;
				else if(move[4] == 'n' && TypeOf(m._inserted) == KNIGHT)
					return m;
				else if(move[4] == 'r' && TypeOf(m._inserted) == ROOK)
					return m;
				else if(move[4] == 'b' && TypeOf(m._inserted) == BISHOP)
					return m;
			}
			else return m;
		}
	}
	
	return Move();
}

void Board::MakeMove(std::string move)
{
	MakeMove(GetMoveFromString(move));
}

std::vector<uint8_t> Board::GetPieceCount()
{
	std::vector<uint8_t> count(13, 0);
	for(int i = 0; i < 64; i++)
	{
		count[_squares[i]]++;
	}
	return count;
}

bool Board::IsGameFinished()
{
	std::vector<Move> moves = GetAllLegalMoves(_current_color);
	if(moves.size() == 0)
	{
		if(IsInCheck(_current_color))
		{
			_game_end_type = CHECKMATE;
			return true;
		}
		_game_end_type = STALEMATE;
		return true;
	}
	else if(_half_move_clock >= 100)
	{
		_game_end_type = FIFTY_MOVE;
		return true;
	}
	else
	{
		std::vector<uint8_t> count = GetPieceCount();
		uint8_t total_b = 0;
		uint8_t total_w = 0;
		for(uint8_t i = RB; i <= PB; i++)
		{
			total_b += count[i];
		}
		for(uint8_t i = RW; i <= PW; i++)
		{
			total_w += count[i];
		}
		if(/*BLACK COUNT*/((total_b == 1 && count[KB] == 1) || (total_b == 2 && count[KB] == 1 && (count[NB] == 1 || count[BB] == 1)))/*BLACK COUNT*/ && /*WHITE COUNT*/((total_w == 1 && count[KW] == 1) || (total_w == 2 && count[KW] == 1 && (count[NW] == 1 || count[BW] == 1)))/*WHITE COUNT*/)
		{
			_game_end_type = DEAD_POSITION;
			return true;
		}
		else if((total_b == 1 && count[KB] == 1 && (total_w == 3 && count[KW] == 1 && count[NW] == 2)) || (total_w == 1 && count[KW] == 1 && (total_b == 3 && count[KB] == 1 && count[NB] == 2)))
		{
			_game_end_type = DEAD_POSITION;
			return true;
		}
	}
	return false;
}