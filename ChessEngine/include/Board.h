#pragma once
#include "ChessDefinitions.h"
#include "ZobristHash.h"

constexpr int piecesNum = 12;

namespace ChessEngine
{

	struct Fen
	{
		std::string board[8];
		bool whiteTurn;
		std::string castling;
		std::string enPassant;
		int halfMove;
		int fullMove;
		Fen(const std::string &FEN);
	};

	struct Move
	{
		ui from;	  // ô bắt đầu
		ui to;		  // ô kết thúc
		ui promotion; // loại quân khi phong cấp (0 = none)
		ui flags;	  // bit flags: capture, en passant, castling, ...

		Move()
			: from(0), to(0), promotion(promoNone), flags(quiet)
		{
		}

		Move(ui fromSq, ui toSq, ui moveFlags = quiet, ui promo = promoNone)
			: from(fromSq), to(toSq), promotion(promo), flags(moveFlags)
		{
		}
	};

	struct MoveList //Danh sách nước 
	{
		Move *begin() { return moves; }
		Move *end() { return moves + size; }

		void push(Move m) { moves[size++] = m; }

	private:
		Move moves[MAX_MOVES];
		int size = 0;
	};

	struct StateInfo
	{
		ui activeColor;
		ui castling;
		ui enPassant;
		ui halfMove;
		ui fullMove;

		ui capturedPiece;
		ui capturedSquare;

		ui moveFlags;
		ui promotedPiece;

		u64 zobristKey;
		ui phaseValue;
		std::array<int, 12> psqtValue;

		StateInfo *previous = nullptr;

		StateInfo();
	};

	struct Board
	{
		u64 pieces[13];	   // Bitboard của các quân
		ui piecesList[64]; // Bàn cờ

		StateInfo *st;							   // current state
		std::array<StateInfo, MAX_PLY> stateStack; // undo stack
		ui ply = 0;

		Board(const Fen &fen);

		void doMove(const Move &move);
		void undoMove(const Move &move);

		u64 computeZobrist(const StateInfo &s) const;
		void printBoard() const;

		bool hasBishopPaired(const Color &side) const;
		bool sufficientMaterialToForceMate(Color &side) const;
		bool fiftyMoveRule() const;
		bool isDrawByRepetition() const;
		bool isDrawByInsufficientMaterial() const;

	private:
		void initBitboardAndList(const Fen &fen);
		void initStateFromFen(const Fen &fen);
	};
}
