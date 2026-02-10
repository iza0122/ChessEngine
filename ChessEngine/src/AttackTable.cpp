#include "AttackTable.h"
#include "MagicBitboard.h"

namespace ChessEngine
{
    vector2D knightDirection[8] = {
        {2, 1},
        {1, 2},
        {-1, 2},
        {-2, 1},
        {-2, -1},
        {-1, -2},
        {1, -2},
        {2, -1}};

    vector2D kingDirection[8] = {
        {1, 0},
        {1, 1},
        {0, 1},
        {-1, 1},
        {-1, 0},
        {-1, -1},
        {0, -1},
        {1, -1}};
}

namespace ChessEngine
{
    AttackTable Attack; //Bảng tra tấn công

    AttackTable::AttackTable()
    {
        rookAttackTable(rookAttack);
        bishopAttackTable(bishopAttack);
        knightAttackTable(knightAttack);
        kingAttackTable(kingAttack);
        pawnAttackTable(pawnAttack);
    }
}

namespace ChessEngine
{
    void knightAttackTable(u64 (&KnightAttack)[64])
    {
        for (int i = 0; i < 64; i++)
        {
            vector2D currentSquare(i % 8, i / 8);
            u64 currentBitboard = 0;
            for (int j = 0; j < 8; j++)
            {
                vector2D sum = currentSquare + ChessEngine::knightDirection[j];
                if (sum.x >= 0 && sum.y >= 0 && sum.x < 8 && sum.y < 8)
                {
                    ChessEngine::setBit(currentBitboard, sum.x + 8 * sum.y);
                }
            }
            KnightAttack[i] = currentBitboard;
        }
    }

    void pawnAttackTable(u64 (&PawnAttack)[2][64])
    {
        memset(PawnAttack, 0, sizeof(PawnAttack));

        for (int i = 0; i < 64; i++)
        {
            int x = i % 8;
            int y = i / 8;

            // Xử lý cả 2 màu trong một lượt duyệt ô cờ
            // Trắng: Tiến lên (y + 1)
            if (y + 1 < 8)
            {
                if (x > 0)
                    ChessEngine::setBit(PawnAttack[White][i], (x - 1) + 8 * (y + 1));
                if (x < 7)
                    ChessEngine::setBit(PawnAttack[White][i], (x + 1) + 8 * (y + 1));
            }

            // Đen: Lùi xuống (y - 1)
            if (y - 1 >= 0)
            {
                if (x > 0)
                    ChessEngine::setBit(PawnAttack[Black][i], (x - 1) + 8 * (y - 1));
                if (x < 7)
                    ChessEngine::setBit(PawnAttack[Black][i], (x + 1) + 8 * (y - 1));
            }
        }
    }

    void kingAttackTable(u64 (&KingAttack)[64])
    {
        for (int i = 0; i < 64; i++)
        {
            vector2D currentSquare(i % 8, i / 8);
            u64 currentBitboard = 0;
            for (int j = 0; j < 8; j++)
            {
                vector2D sum = currentSquare + kingDirection[j];
                if (sum.x >= 0 && sum.y >= 0 && sum.x < 8 && sum.y < 8)
                {
                    ChessEngine::setBit(currentBitboard, sum.x + 8 * sum.y);
                }
            }
            KingAttack[i] = currentBitboard;
        }
    }
}