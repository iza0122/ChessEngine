#pragma once
#include "ChessDefinitions.h"
#include "Ultilities.h"

namespace ChessEngine {
	struct Zobrist {
		u64 pieces[12][64];
		u64 sideToMove;
		u64 castlingRight[4];
		u64 enPassant[8];

		Zobrist();
	};

	extern Zobrist zobrist;
}
