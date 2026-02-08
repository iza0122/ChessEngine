#include "MagicBitboard.h"

vector2D bishopDirection[4] = {
    {1,1},
    {-1,1},
    {1,-1},
    {-1,-1}
};


/**
 * @brief Generates the blocker mask for a rook on a given square.
 *
 * This function computes a bitboard mask representing all squares that could potentially block
 * a rook's movement from the specified index on a standard 8x8 chessboard. The mask excludes
 * edge squares, considering only the squares between the rook and the edge in each direction.
 *
 * @param index The square index (0-63) where the rook is located.
 * @return u64 Bitboard mask of potential blocker squares for the rook.
 */
u64 rookBlockerMask(ui index)
{
    u64 blockerMask = C64(0);
    int row = index / 8;
    int col = index % 8;
    for (int i = 1; i < 7 - row; i++) {
        ChessEngine::setBit(blockerMask, index + i * 8);
    }
    for (int i = row - 1; i > 0; i--) {
        ChessEngine::setBit(blockerMask, index - i * 8);
    }
    for (int i = 1; i < 7 - col; i++) {
        ChessEngine::setBit(blockerMask, index + i);
    }
    for (int i = col - 1; i > 0; i--) {
        ChessEngine::setBit(blockerMask, index - i);
    }
    return blockerMask;
}

/**
 * @brief Generates all possible blocker board combinations from a given blocker mask.
 *
 * Given a bitboard mask representing blocker squares, this function computes all possible
 * subsets of blockers (as bitboards) that can be formed from the set bits in the mask.
 * Each subset corresponds to a possible arrangement of blockers on the specified squares.
 *
 * @param blockerMask A 64-bit unsigned integer representing the blocker mask (bitboard).
 * @return std::vector<u64> A vector containing all possible blocker bitboards derived from the mask.
 */
std::vector<u64> blockerBoard(const u64& blockerMask)
{
    std::vector<int> squares;
    std::vector<u64> result;
    for (int sq = 0; sq < 64; sq++) {
        if (blockerMask & (1ULL << sq)) squares.push_back(sq);
    }
    size_t n = squares.size();
    int subsets = 1 << n;
    for (int index = 0; index < subsets; index++) {
        u64 blockers = 0ULL;
        for (int i = 0; i < n; i++) {
            if (index & (1 << i)) {
                blockers |= (1ULL << squares[i]);
            }
        }
        result.push_back(blockers);
    }
    return result;
}

/**
 * @brief Generates the blocker mask for a bishop on a given square.
 *
 * This function calculates the mask representing all squares that could block
 * the movement of a bishop from the specified index on a chessboard. The mask
 * excludes edge squares and only includes squares between the bishop and the edge
 * in all four diagonal directions.
 *
 * @param index The square index (0-63) for which to generate the blocker mask.
 * @return u64 The blocker mask as a 64-bit unsigned integer.
 */
u64 bishopBlockerMask(ui index)
{
    u64 blockerMask = C64(0);
    vector2D currentPosition(index % 8, index / 8);
    for (int i = 1; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
            vector2D updatedDirection(bishopDirection[j].x * i, bishopDirection[j].y * i);
            vector2D sum = currentPosition + updatedDirection;
            if (sum.x > 0 && sum.y > 0 && sum.x < 7 && sum.y < 7) {
                ChessEngine::setBit(blockerMask, sum.x + 8 * sum.y);
            }
        }
    }
    return blockerMask;
}


/**
 * @brief Returns the attack bitboard for a rook on a given square with a given occupancy.
 *
 * This function calculates all squares a rook can attack from the specified square,
 * considering the current occupancy bitboard. The rook's movement is blocked by any
 * piece encountered in each direction.
 *
 * @param square The board square (0-63) where the rook is located.
 * @param occupancy The occupancy bitboard representing blocked squares.
 * @return u64 Bitboard of all squares attacked by the rook.
 */
u64 getRookAttack(int square, u64 occupancy) {
    u64 attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Move north
    for (int r = rank + 1; r <= 7; r++) {
        int sq = r * 8 + file;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // Move south
    for (int r = rank - 1; r >= 0; r--) {
        int sq = r * 8 + file;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // Move east
    for (int f = file + 1; f <= 7; f++) {
        int sq = rank * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // Move west
    for (int f = file - 1; f >= 0; f--) {
        int sq = rank * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    return attacks;
}

/**
 * @brief Returns the attack bitboard for a bishop on a given square with a given occupancy.
 *
 * This function calculates all squares a bishop can attack from the specified square,
 * considering the current occupancy bitboard. The bishop's movement is blocked by any
 * piece encountered in each diagonal direction.
 *
 * @param square The board square (0-63) where the bishop is located.
 * @param occupancy The occupancy bitboard representing blocked squares.
 * @return u64 Bitboard of all squares attacked by the bishop.
 */
u64 getBishopAttack(int square, u64 occupancy) {
    u64 attacks = 0ULL;
    int rank = square / 8;
    int file = square % 8;

    // Diagonal northeast (NE)
    for (int r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++) {
        int sq = r * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // Diagonal northwest (NW)
    for (int r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--) {
        int sq = r * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // Diagonal southeast (SE)
    for (int r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++) {
        int sq = r * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // Diagonal southwest (SW)
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--) {
        int sq = r * 8 + f;
        attacks |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    return attacks;
}

void rookAttackTable(u64(&rookAttackTable)[64][4096])
{
    memset(rookAttackTable, 0, sizeof rookAttackTable);
    for (int square = 0; square < 64; square++) {
        u64 rookBlocker = rookBlockerMask(square);
        std::vector<u64> rookOccupancy = blockerBoard(rookBlocker);
        for (int size = 0; size < rookOccupancy.size(); size++) {
            int index = (rookOccupancy[size] * rookMagic[square]) >> rookShift[square];
            u64 rookAttack = getRookAttack(square, rookOccupancy[size]);
            rookAttackTable[square][index] = rookAttack;
        }
    }
}

void bishopAttackTable(u64(&bishopAttackTable)[64][512])
{
    memset(bishopAttackTable, 0, sizeof bishopAttackTable);
    for (int square = 0; square < 64; square++) {
        u64 bishopBlocker = bishopBlockerMask(square);
        std::vector<u64> bishopOccupancy = blockerBoard(bishopBlocker);
        for (int size = 0; size < bishopOccupancy.size(); size++) {
            int index = (bishopOccupancy[size] * bishopMagic[square]) >> bishopShift[square];
            u64 bishopAttack = getBishopAttack(square, bishopOccupancy[size]);
            bishopAttackTable[square][index] = bishopAttack;
        }
    }
}

