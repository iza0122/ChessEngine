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
		// Nếu FEN báo không có ô EnPassant, trả về 64 (NoSquare)
		if (fenField == "-") return 64;

		// FEN quy định ô EP phải có đủ 2 ký tự (ví dụ: "e3")
		int file = fenField[0] - 'a'; // Cột a-h -> 0-7
		int rank = fenField[1] - '1'; // Hàng 1-8 -> 0-7

		// Trả về chỉ số ô từ 0-63
		// Ví dụ: "e3" -> rank 2, file 4 -> 2 * 8 + 4 = 20
		return rank * 8 + file;
	}

	ui promotePiece(const ui &pawn,const ui &promo)
	{
		bool isWhite = pawn <= WhiteKing;

		switch (promo) {
		case promoKnight: return isWhite ? WhiteKnight : BlackKnight;
		case promoBishop: return isWhite ? WhiteBishop : BlackBishop;
		case promoRook:   return isWhite ? WhiteRook : BlackRook;
		case promoQueen:  return isWhite ? WhiteQueen : BlackQueen;
		default: return pawn;
		}
	}

	ui unpromotePiece(const ui &promotedPiece)
	{
		// White promotion
		if (promotedPiece >= WhiteKnight && promotedPiece <= WhiteQueen)
			return WhitePawn;

		// Black promotion
		if (promotedPiece >= BlackKnight && promotedPiece <= BlackQueen)
			return BlackPawn;

		// Không phải promotion (defensive)
		return promotedPiece;
	}

}
