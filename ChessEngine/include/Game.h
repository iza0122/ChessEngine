#pragma once
#include "ChessDefinitions.h"

#define piecesNum 12


namespace ChessEngine {


	struct Fen {
		std::string board[8];
		bool whiteTurn;
		std::string castling;
		std::string enPassant;
		int halfMove;
		int fullMove;
		Fen(const std::string& FEN);
	};

	struct Board {
		u64 pieces[12];
		u64 occupied;
		Board();
		void setupFromFen(const Fen& fen);
		void printBoard();
	};

	class Game {
	public:
		Board board;
		bool whiteTurn;
		std::string castling;
		std::string enPassant;
		int halfMove;
		int fullMove;
	public:
		bool LoadGame(const std::string& filename, const std::string& mode);
		// Other game logic
	};
}
