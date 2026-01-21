#include "Ultilities.h"
#include <bit>
#include <iostream>

namespace ChessEngine {

	// ===== vector2D =====
	vector2D::vector2D(int X, int Y) : x(X), y(Y) {}

	vector2D vector2D::operator+(const vector2D& other) const {
		return vector2D(x + other.x, y + other.y);
	}

	vector2D vector2D::operator-(const vector2D& other) const {
		return vector2D(x - other.x, y - other.y);
	}

	// ===== Bitboard helpers =====
	void setBit(u64& bitboard, ui index) {
		bitboard |= C64(1) << index;
	}

	void toggleBit(u64& bitboard, ui index) {
		bitboard ^= C64(1) << index;
	}

	void resetBit(u64& bitboard, ui index) {
		bitboard &= ~(C64(1) << index);
	}

	bool testBit(const u64& bitboard, ui index) {
		return bitboard & (C64(1) << index);
	}

	// ===== Bit masks =====
	u64 upperBits(ui index) {
		return (C64(~1) << index);
	}

	u64 lowerBits(ui index) {
		return (C64(1) << index) - 1;
	}

	// ===== Bit operations =====
	int popcount(const u64& bitboard) {
		return std::popcount(bitboard);
	}

	int hammingDistance(const u64& obj1, const u64& obj2) {
		return std::popcount(obj1 ^ obj2);
	}

	// ===== Debug =====
	void printBitboard(const u64& bitboard) {
		for (int rank = 7; rank >= 0; --rank) {
			for (int file = 0; file < 8; ++file) {
				int sq = rank * 8 + file;
				std::cout << ((bitboard & (C64(1) << sq)) ? "1 " : ". ");
			}
			std::cout << '\n';
		}
		std::cout << std::endl;
	}

	// ===== FEN =====
	int parseEnPassant(const std::string& fenField) {
		if (fenField == "-") return 8; // không có en passant
		return fenField[0] - 'a';      // 0..7
	}

}
