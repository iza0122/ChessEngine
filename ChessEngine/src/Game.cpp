#include "Game.h"
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


void ChessEngine::Board::doMove(const Move& move)
{
	StateInfo* newSt = &stateStack[++ply];

	*newSt = *st;
	newSt->previous = st;
	st = newSt;

	ui from = move.from;
	ui to = move.to;

	ui movingPiece = piecesList[from];
	ui capturedPiece = piecesList[to];

	piecesList[from] = NoPiece;
	resetBit(pieces[movingPiece], from);

	//Capture
	if (move.flags & capture) {
		//En-passant
		if (move.flags & enPassant) {
			ui capturedSquare = (st->activeColor == White) ? to - 8 : to + 8;
			capturedPiece = piecesList[capturedSquare];
			piecesList[capturedSquare] = NoPiece;
			resetBit(pieces[capturedPiece], capturedSquare);
		}
		else if (capturedPiece != NoPiece) {
			resetBit(pieces[capturedPiece], to);
		}
	}

	//Promotion
	if (move.flags & promotion) {
		movingPiece = promotePiece(movingPiece, move.promotion);
	}

	//Castling
	if (move.flags & castling) {
		if (to == g1 || to == g8) {
			ui rookFrom = to + 1;
			ui rookTo = to - 1;
			ui rook = piecesList[rookFrom];

			piecesList[rookFrom] = NoPiece;
			resetBit(pieces[rook], rookFrom);

			piecesList[rookTo] = rook;
			setBit(pieces[rook], rookTo);
		}
		else if (to == c1 || to == c8) {
			ui rookFrom = to - 2;
			ui rookTo = to + 1;
			ui rook = piecesList[rookFrom];

			piecesList[rookFrom] = NoPiece;
			resetBit(pieces[rook], rookFrom);

			piecesList[rookTo] = rook;
			setBit(pieces[rook], rookTo);
		}
	}

	piecesList[to] = movingPiece;
    setBit(pieces[movingPiece], to);

	st->enPassant = -1;
	if (move.flags & doublePush) {
		st->enPassant = (from + to) / 2;
	}

	st->activeColor ^= 1;
	st->zobristKey ^= zobrist.sideToMove;
}

void ChessEngine::Board::undoMove() {
	st = st->previous;
	--ply;
}

ChessEngine::StateInfo::StateInfo()
	: activeColor(1)     // Mặc định White = 1
	, castling(0)        // Mặc định không có quyền nhập thành
	, enPassant(64)      // 64 nghĩa là không có ô EP (NoSquare)
	, halfMove(0)
	, fullMove(1)
	, zobristKey(0ULL)
	, phaseValue(0)
	, previous(nullptr)
{
	// Khởi tạo mảng PSQT về 0
	psqtValue.fill(0);
}
