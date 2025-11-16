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

	struct GameState {
		ui activeColor; //Black = 0, White = 1
		ui castling;
		ui enPassant;
		ui halfMove;
		ui fullMove;

		u64 zobristKey;
		ui phaseValue;
		std::array<int, 12> psqtValue;
		Move nextMove;
	};

	struct History {
		std::array<GameState, MAX_MOVE> history;
		ui size = 0;
		void push(const GameState& gamestate) {
			history[size] = gamestate;
			size++;
		}

		void pop() {
			if (size == 0) return;
			size--;
		}

		GameState& get_state(const ui& pos) {
			return history[pos];
		}

		ui len() {
			return size;
		}

		void clear() {
			size = 0;
		}
	};

	struct Board {
		u64 pieces[12];
		ui piecesList[64]; //Pieces list

		GameState gameState;
		History history;

		Board();
		void initBitboardAndList(const Fen& fen);
		void initGameState(const Fen& fen);
		u64 computeZobrist();
		void printBoard();
	};
}
