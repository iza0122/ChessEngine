#include "Board.h"
#include "Ultilities.h"

ChessEngine::Fen::Fen(const std::string& FEN)
{
	std::istringstream FENStream(FEN);
	std::string t_board, t_turn;
	FENStream >> t_board >> t_turn >> castling >> enPassant >> halfMove >> fullMove;
	std::istringstream boardStream(t_board);
	for (int i = 0; i < 8; i++) {
		getline(boardStream, board[i], '/');
	}
	if (t_turn == "w") whiteTurn = true;
	else whiteTurn = false;
}


ChessEngine::Board::Board(const Fen& fen) {
	for (size_t i = 0; i < 13; i++) {
		pieces[i] = NoPiece;
	}
	for (size_t i = 0; i < 64; i++) {
		piecesList[i] = NoPiece;
	}
	initBitboardAndList(fen);

	ply = 0;
	st = &stateStack[0];

	initStateFromFen(fen);
}

//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

void ChessEngine::Board::initBitboardAndList(const Fen& fen)
{
	const std::string* boardData = fen.board;
	int position = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < boardData[7 - i].size(); j++)
		{
			char x = boardData[7 - i][j];
			if (isdigit(x))
			{
				position += (x - '0');
			}
			else
			{
				switch (x)
				{
                case 'p': setBit(this->pieces[BlackPawn], position); piecesList[position++] = BlackPawn; break;
                case 'P': setBit(this->pieces[WhitePawn], position); piecesList[position++] = WhitePawn; break;
                case 'r': setBit(this->pieces[BlackRook], position); piecesList[position++] = BlackRook; break;
                case 'R': setBit(this->pieces[WhiteRook], position); piecesList[position++] = WhiteRook; break;
                case 'n': setBit(this->pieces[BlackKnight], position); piecesList[position++] = BlackKnight; break;
                case 'N': setBit(this->pieces[WhiteKnight], position); piecesList[position++] = WhiteKnight; break;
                case 'b': setBit(this->pieces[BlackBishop], position); piecesList[position++] = BlackBishop; break;
                case 'B': setBit(this->pieces[WhiteBishop], position); piecesList[position++] = WhiteBishop; break;
                case 'q': setBit(this->pieces[BlackQueen], position); piecesList[position++] = BlackQueen; break;
                case 'Q': setBit(this->pieces[WhiteQueen], position); piecesList[position++] = WhiteQueen; break;
                case 'k': setBit(this->pieces[BlackKing], position); piecesList[position++] = BlackKing; break;
                case 'K': setBit(this->pieces[WhiteKing], position); piecesList[position++] = WhiteKing; break;
                default: position++; break;
				}
			}
		}
	}
	
}

void ChessEngine::Board::initStateFromFen(const Fen& fen)
{
	StateInfo& s = stateStack[0];

	// Color
	s.activeColor = (fen.whiteTurn == White) ? White : Black;

	// Castling
	s.castling = 0;
	if (fen.castling.find('K') != std::string::npos) s.castling |= 1;
	if (fen.castling.find('Q') != std::string::npos) s.castling |= 2;
	if (fen.castling.find('k') != std::string::npos) s.castling |= 4;
	if (fen.castling.find('q') != std::string::npos) s.castling |= 8;

	// En passant
	s.enPassant = parseEnPassant(fen.enPassant);

	// Move counters
	s.halfMove = fen.halfMove;
	s.fullMove = fen.fullMove;

	// Zobrist
	s.zobristKey = computeZobrist(s);

	// Init others
	s.phaseValue = 0;
	s.psqtValue.fill(0);

	s.previous = nullptr;
}


u64 ChessEngine::Board::computeZobrist(const StateInfo& s) const
{
	u64 key = 0;

	// Pieces
	for (int sq = 0; sq < 64; sq++) {
		int piece = piecesList[sq];
		if (piece != NoPiece)
			key ^= zobrist.pieces[piece][sq];
	}

	// En passant
	if (s.enPassant < 64) {
		key ^= zobrist.enPassant[s.enPassant % 8];
	}

	// Castling
	if (s.castling & 1) key ^= zobrist.castlingRight[0];
	if (s.castling & 2) key ^= zobrist.castlingRight[1];
	if (s.castling & 4) key ^= zobrist.castlingRight[2];
	if (s.castling & 8) key ^= zobrist.castlingRight[3];

	// Side to move
	if (s.activeColor == Black)
		key ^= zobrist.sideToMove;

	return key;
}


void ChessEngine::Board::printBoard() const{
	// Ký tự đại diện cho từng loại quân
	const char pieceChar[13] = {
		'P', 'N', 'B', 'R', 'Q', 'K',   // White
		'p', 'n', 'b', 'r', 'q', 'k' , '.'   // Black
	};


	for (int rank = 7; rank >= 0; rank--) {

		for (int file = 0; file < 8; file++) {
			int sq = rank * 8 + file;   // 0..63 index
			char c = pieceChar[piecesList[sq]];
			std::cout << c << ' ';
		}

		std::cout << rank + 1 << "\n";
	}

	std::cout << "a b c d e f g h\n\n";
}

bool ChessEngine::Board::hasBishopPaired(const Color &side) const
{
    ui bishopType = (side == White) ? WhiteBishop : BlackBishop;
    u64 bishopBB = pieces[bishopType];

    return (bishopBB & DarkSquares) && (bishopBB & LightSquares);
}

bool ChessEngine::Board::sufficientMaterialToForceMate(Color& side) const
{
	if (pieces[WhitePawn] | pieces[BlackPawn] |
		pieces[WhiteRook] | pieces[BlackRook] |
		pieces[WhiteQueen] | pieces[BlackQueen])
		return true;

	// Bishop pair (khác màu)
	if (hasBishopPaired(White) || hasBishopPaired(Black))
		return true;

	// Bishop + Knight
	if ((pieces[WhiteBishop] && pieces[WhiteKnight]) ||
		(pieces[BlackBishop] && pieces[BlackKnight]))
		return true;

	return false;
}

bool ChessEngine::Board::fiftyMoveRule() const
{
	return st->halfMove >= MAX_MOVE_RULE;
}

bool ChessEngine::Board::isDrawByRepetition() const {
	int count = 0;
	u64 key = st->zobristKey;

	for (StateInfo* s = st; s != nullptr; s = s->previous) {
		if (s->zobristKey == key) {
			if (++count >= 3)
				return true;
		}

		// Pawn move hoặc capture → không thể lặp trước đó
		if (s->halfMove == 0)
			break;
	}

	return false;
}

bool ChessEngine::Board::isDrawByInsufficientMaterial() const
{
	if (pieces[WhitePawn] || pieces[BlackPawn])  return false;
	if (pieces[WhiteRook] || pieces[BlackRook])  return false;
	if (pieces[WhiteQueen] || pieces[BlackQueen]) return false;

	int wN = popcount(pieces[WhiteKnight]);
	int bN = popcount(pieces[BlackKnight]);
	int wB = popcount(pieces[WhiteBishop]);
	int bB = popcount(pieces[BlackBishop]);

	int totalMinors = wN + bN + wB + bB;

	// K vs K
	if (totalMinors == 0)
		return true;

	// K + 1 minor vs K
	if (totalMinors == 1)
		return true;

	// K+B vs K+B (cùng màu ô)
	if (totalMinors == 2 && wB == 1 && bB == 1) {
		u64 wB = pieces[WhiteBishop];
		u64 bB = pieces[BlackBishop];

		// White bishop trên dark?
		bool wDark = (wB & DarkSquares) != 0;
		bool wLight = (wB & LightSquares) != 0;

		// Black bishop trên dark?
		bool bDark = (bB & DarkSquares) != 0;
		bool bLight = (bB & LightSquares) != 0;

		// Cùng màu ô
		return (wDark && bDark) || (wLight && bLight);
	}

	return false;
}



void ChessEngine::Board::doMove(const Move& move)
{
	StateInfo* newSt = &stateStack[++ply];

	*newSt = *st;
	newSt->previous = st;
	st = newSt;

	ui from = move.from;
	ui to = move.to;

	st->moveFlags = move.flags;
	st->promotedPiece = NoPiece;

	ui movingPiece = piecesList[from];
	ui capturedPiece = piecesList[to];
	ui capturedSquare = to;

	st->capturedPiece = NoPiece;
	st->capturedSquare = 64;

	// ===== Remove moving piece from FROM =====
	piecesList[from] = NoPiece;
	resetBit(pieces[movingPiece], from);
	st->zobristKey ^= zobrist.pieces[movingPiece][from];

	// ===== Remove old en-passant =====
	if (st->previous->enPassant != 64)
		st->zobristKey ^= zobrist.enPassant[st->previous->enPassant % 8];

	// ===== Remove old castling =====
	st->zobristKey ^= zobrist.castlingRight[st->previous->castling];

	// ===== Capture =====
	if (move.flags & capture) {
		if (move.flags & enPassant) {
			capturedSquare = (st->activeColor == White) ? to - 8 : to + 8;
			capturedPiece = piecesList[capturedSquare];
		}

		st->capturedPiece = capturedPiece;
		st->capturedSquare = capturedSquare;

		piecesList[capturedSquare] = NoPiece;
		resetBit(pieces[capturedPiece], capturedSquare);
		st->zobristKey ^= zobrist.pieces[capturedPiece][capturedSquare];
	}

	// ===== Promotion =====
	if (move.flags & promotion) {
		st->promotedPiece = promotePiece(movingPiece, move.promotion);
		movingPiece = st->promotedPiece;
	}

	// ===== Castling =====
	if (move.flags & castling) {
		ui rookFrom, rookTo;

		if (to == g1 || to == g8) {
			rookFrom = to + 1;
			rookTo = to - 1;
		}
		else {
			rookFrom = to - 2;
			rookTo = to + 1;
		}

		ui rook = piecesList[rookFrom];

		piecesList[rookFrom] = NoPiece;
		resetBit(pieces[rook], rookFrom);
		st->zobristKey ^= zobrist.pieces[rook][rookFrom];

		piecesList[rookTo] = rook;
		setBit(pieces[rook], rookTo);
		st->zobristKey ^= zobrist.pieces[rook][rookTo];
	}

	// ===== Place moving piece to TO =====
	piecesList[to] = movingPiece;
	setBit(pieces[movingPiece], to);
	st->zobristKey ^= zobrist.pieces[movingPiece][to];

	// ===== Update castling rights =====
	st->castling &= castleMask[from];
	st->castling &= castleMask[to];
	st->zobristKey ^= zobrist.castlingRight[st->castling];

	// ===== En-passant =====
	st->enPassant = NoSquare;
	if (move.flags & doublePush) {
		st->enPassant = (from + to) / 2;
		st->zobristKey ^= zobrist.enPassant[st->enPassant % 8];
	}

	// ===== Halfmove clock =====
	if (move.flags & capture || movingPiece == WhitePawn || movingPiece == BlackPawn)
		st->halfMove = 0;
	else
		st->halfMove++;

	// ===== Side to move =====
	st->activeColor ^= 1;
	st->zobristKey ^= zobrist.sideToMove;

	// ===== Fullmove =====
	if (st->activeColor == White)
		st->fullMove++;
}


void ChessEngine::Board::undoMove(const Move& move)
{
    StateInfo* cur = st;
    st = st->previous;
    ply--;

    ui from = move.from;
    ui to   = move.to;

    ui movingPiece = piecesList[to];

    // ===== Side to move =====
    // (doMove đã flip, undo phải flip lại)
    st->activeColor ^= 1;

    // ===== Remove moving piece from TO =====
    piecesList[to] = NoPiece;
    resetBit(pieces[movingPiece], to);

    // ===== Undo promotion =====
    if (cur->moveFlags & promotion) {
        movingPiece = unpromotePiece(movingPiece);
    }

    // ===== Place piece back to FROM =====
    piecesList[from] = movingPiece;
    setBit(pieces[movingPiece], from);

    // ===== Undo castling =====
    if (cur->moveFlags & castling) {
        ui rookFrom, rookTo;

        if (to == g1 || to == g8) {
            rookFrom = to + 1;
            rookTo   = to - 1;
        } else {
            rookFrom = to - 2;
            rookTo   = to + 1;
        }

        ui rook = piecesList[rookTo];

        piecesList[rookTo] = NoPiece;
        resetBit(pieces[rook], rookTo);

        piecesList[rookFrom] = rook;
        setBit(pieces[rook], rookFrom);
    }

    // ===== Restore captured piece =====
    if (cur->capturedPiece != NoPiece) {
        piecesList[cur->capturedSquare] = cur->capturedPiece;
        setBit(pieces[cur->capturedPiece], cur->capturedSquare);
    }

    // Zobrist, castling, en-passant, clocks
    // đã được restore hoàn toàn bằng StateInfo
}

ChessEngine::StateInfo::StateInfo()
	: activeColor(1)     // Mặc định White = 1
	, castling(0)        // Mặc định không có quyền nhập thành
	, enPassant(NoSquare)      // 64 nghĩa là không có ô EP (NoSquare)
	, halfMove(0)
	, fullMove(1)
	, zobristKey(0ULL)
	, phaseValue(0)
	, previous(nullptr)
	, capturedPiece(NoPiece)
	, capturedSquare(NoSquare)
	, moveFlags(quiet)
	, promotedPiece(promoNone)
{
	// Khởi tạo mảng PSQT về 0
	psqtValue.fill(0);
}

