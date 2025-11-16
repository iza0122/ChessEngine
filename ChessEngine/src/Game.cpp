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


ChessEngine::Board::Board()
{
	for (size_t i = 0; i < 12; i++) {
		pieces[i] = 0;
	}
	for (size_t i = 0; i < 64; i++) {
		piecesList[i] = 0;
	}
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

void ChessEngine::Board::initGameState(const Fen& fen)
{
	//Color
	if (fen.whiteTurn == 'w') {
		gameState.activeColor = White;
	}
	else {
		gameState.activeColor = Black;
	}

	//Castling
	if (fen.castling.find('K') != std::string::npos) gameState.castling |= 1;
	if (fen.castling.find('Q') != std::string::npos) gameState.castling |= 2;
	if (fen.castling.find('k') != std::string::npos) gameState.castling |= 4;
	if (fen.castling.find('q') != std::string::npos) gameState.castling |= 8;
	
	//En Passant
	gameState.enPassant = parseEnPassant(fen.enPassant);

	//Move count
	gameState.halfMove = fen.halfMove;
	gameState.fullMove = fen.fullMove;

	//Zobrist
	gameState.zobristKey = computeZobrist();

	////Phase Value
	//gameState.phaseValue;
	////PQST table
	//gameState.psqtValue;
}

u64 ChessEngine::Board::computeZobrist()
{
	u64 key = 0;
	for (int i = 0; i < 64; i++) {
		int piece = piecesList[i];
		if (piece != NoPiece) key ^= zobrist.pieces[piece][i];
	}

	if (gameState.enPassant < 8) {
		key ^= zobrist.enPassant[gameState.enPassant];
	}

	if (gameState.castling & 1) key ^= zobrist.castlingRight[0]; // K
	if (gameState.castling & 2) key ^= zobrist.castlingRight[1]; // Q
	if (gameState.castling & 4) key ^= zobrist.castlingRight[2]; // k
	if (gameState.castling & 8) key ^= zobrist.castlingRight[3]; // q

	if (gameState.activeColor == Black) {
		key ^= zobrist.sideToMove;
	}

	return key;
}

void ChessEngine::Board::printBoard() {
	// Ký tự đại diện cho từng loại quân
	const char pieceChar[12] = {
		'P', 'N', 'B', 'R', 'Q', 'K',   // White
		'p', 'n', 'b', 'r', 'q', 'k'    // Black
	};


	for (int rank = 7; rank >= 0; rank--) {

		for (int file = 0; file < 8; file++) {
			int sq = rank * 8 + file;   // 0..63 index

			char c = '.';  // ô trống

			for (int i = 0; i < 12; i++) {
				if (pieces[i] & (1ULL << sq)) {
					c = pieceChar[i];
					break;
				}
			}

			std::cout << c << ' ';
		}

		std::cout << rank + 1 << "\n";
	}

	std::cout << "a b c d e f g h\n\n";
}


ChessEngine::Move::Move()
{
	flags = 0;
	from = 0;
	promotion = 0;
	to = 0;
}
