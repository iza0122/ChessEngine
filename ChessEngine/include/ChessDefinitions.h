#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <bit>
#include <bitset>
#include <cstdint>
#include <array>
#include <random>

constexpr int MAX_MOVES = 256; //Kích thước của danh sách nước
constexpr int MAX_PLY = 256;
constexpr int MAX_MOVE_RULE = 100;

//Define bitboard
using u64 = unsigned long long;
using ui = unsigned int;
#define C64(constantU64) constantU64##ULL

//Pieces
enum Piece : unsigned int {
	WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
	BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing,
	NoPiece
};

struct vector2D {
	int x, y;
	vector2D(int X = 0, int Y = 0) : x(X), y(Y) {}
	vector2D operator+(const vector2D& other) const {
		return vector2D(x + other.x, y + other.y);
	}
	vector2D operator-(const vector2D& other) const {
		return vector2D(x - other.x, y - other.y);
	}
};

enum Color : ui {
	Black = 0, White = 1
};


enum direction : int {
	N = 8,
	NE = 9,
	E = 1,
	SE = -7,
	S = -8,
	SW = -9,
	W = -1,
	NW = 7,
};

enum enumSquare : ui {
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8, NoSquare
};

enum MoveFlag : ui {
    quiet        = 0,
    capture      = 1 << 0,
    doublePush  = 1 << 1,
    enPassant   = 1 << 2,
    castling    = 1 << 3,
    promotion   = 1 << 4
};

enum PromotionPiece : ui {
    promoNone = 0,
    promoKnight,
    promoBishop,
    promoRook,
    promoQueen
};



//Bitboard constants

constexpr u64 AFile = C64(0x0101010101010101);
constexpr u64 HFile = C64(0x8080808080808080);
constexpr u64 Rank1 = C64(0x00000000000000ff);
constexpr u64 Rank8 = C64(0xff00000000000000);
constexpr u64 A1H8Diagonal = C64(0x8040201008040201);
constexpr u64 H1A8Antidiagonal = C64(0x0102040810204080);
constexpr u64 LightSquares = C64(0x55aa55aa55aa55aa);
constexpr u64 DarkSquares = C64(0xaa55aa55aa55aa55);
constexpr u64 Empty = C64(0x0000000000000000);
constexpr u64 Universe = C64(0xffffffffffffffff);
constexpr u64 Border = C64(0xff818181818181ff);

//Castle Mask
const ui castleMask[64] = {
	13, 15, 15, 15, 12, 15, 15, 14, // H�ng 1 (a1-h1): a1=13 (1101), e1=12 (1100), h1=14 (1110)
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	 7, 15, 15, 15,  3, 15, 15, 11  // H�ng 8 (a8-h8): a8=7 (0111), e8=3 (0011), h8=11 (1011)
};
