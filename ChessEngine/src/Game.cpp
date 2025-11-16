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
	occupied = 0;
}
//rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

void ChessEngine::Board::setupFromFen(const Fen& fen)
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
				case 'p': setBit(this->pieces[BlackPawn], position++); break;
				case 'P': setBit(this->pieces[WhitePawn], position++); break;
				case 'r': setBit(this->pieces[BlackRook], position++); break;
				case 'R': setBit(this->pieces[WhiteRook], position++); break;
				case 'n': setBit(this->pieces[BlackKnight], position++); break;
				case 'N': setBit(this->pieces[WhiteKnight], position++); break;
				case 'b': setBit(this->pieces[BlackBishop], position++); break;
				case 'B': setBit(this->pieces[WhiteBishop], position++); break;
				case 'q': setBit(this->pieces[BlackQueen], position++); break;
				case 'Q': setBit(this->pieces[WhiteQueen], position++); break;
				case 'k': setBit(this->pieces[BlackKing], position++); break;
				case 'K': setBit(this->pieces[WhiteKing], position++); break;
				default: position++; break;
				}
			}
		}
	}
	occupied = 0;
	for (size_t i = 0; i < 12; i++)
	{
		occupied |= pieces[i];
	}
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
	printBitboard(this->occupied);
}

bool ChessEngine::Game::LoadGame(const std::string& filename, const std::string& mode)
{
	if (mode == "FEN") {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Cannot open file!\n";
			return false;
		}

		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();

		Fen fen(content);
		board.setupFromFen(fen);
		whiteTurn = fen.whiteTurn;
		castling = fen.castling;
		enPassant = fen.enPassant;
		halfMove = fen.halfMove;
		fullMove = fen.fullMove;
		return true;
	}
	return false;
}
