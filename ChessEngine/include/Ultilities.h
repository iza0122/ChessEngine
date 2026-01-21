#pragma once
#include "ChessDefinitions.h"
#include <string>

namespace ChessEngine {

	// Define vector
	struct vector2D {
		int x, y;
		vector2D(int X = 0, int Y = 0);
		vector2D operator+(const vector2D& other) const;
		vector2D operator-(const vector2D& other) const;
	};

	// Bitboard helpers
	void setBit(u64& bitboard, ui index);
	void toggleBit(u64& bitboard, ui index);
	void resetBit(u64& bitboard, ui index);
	bool testBit(const u64& bitboard, ui index);

	// Bit masks
	u64 upperBits(ui index);
	u64 lowerBits(ui index);

	// Bit operations
	int popcount(const u64& bitboard);
	int hammingDistance(const u64& obj1, const u64& obj2);

	// Debug / utils
	void printBitboard(const u64& bitboard);
	int parseEnPassant(const std::string& fenField);

}
