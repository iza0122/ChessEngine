#include "ZobristHash.h"

namespace ChessEngine{
	Zobrist zobrist; //Khởi tạo duy nhất 
}

ChessEngine::Zobrist::Zobrist()
{
	std::mt19937_64 rng(20012006);
	std::uniform_int_distribution<uint64_t> dist;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 64; j++) {
			pieces[i][j] = dist(rng);
		}
	}

	sideToMove = dist(rng);

	for (int i = 0; i < 4; i++) {
		castlingRight[i] = dist(rng);
	}

	for (int i = 0; i < 8; i++) {
		enPassant[i] = dist(rng);
	}
}

