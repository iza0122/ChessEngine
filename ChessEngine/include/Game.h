#pragma once
#include "ChessDefinitions.h"
#include "ZobristHash.h"

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

	struct Move {
		ui from;       // ô bắt đầu
		ui to;         // ô kết thúc
		ui promotion;  // loại quân khi phong cấp (0 = none)
		ui flags;      // bit flags: capture, en passant, castling, ...

		Move();
	};

	struct StateInfo {
		ui activeColor;      // 0 = black, 1 = white
		ui castling;
		ui enPassant;
		ui halfMove;
		ui fullMove;

		u64 zobristKey;
		ui phaseValue;
		std::array<int, 12> psqtValue;

		StateInfo* previous = nullptr;
	};
	

	struct Board {
		u64 pieces[12];
		ui piecesList[64];

		StateInfo* st;                              // current state
		std::array<StateInfo, MAX_PLY> stateStack; // undo stack
		ui ply = 0;

		Board(const Fen& fen);

		void doMove(const Move& m);
		void undoMove();

		u64 computeZobrist(const StateInfo& s) const;
		void printBoard();
	private:
		void initBitboardAndList(const Fen& fen);
		void initStateFromFen(const Fen& fen);
	};
}
