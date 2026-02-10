#include "ChessDefinitions.h"


namespace ChessEngine {

	struct AttackTable {
		u64 pawnAttack[2][64];
		u64 kingAttack[64];
		u64 knightAttack[64];
		u64 rookAttack[64][4096];
		u64 bishopAttack[64][512];

		AttackTable();
	};

    extern AttackTable Attack;
    void knightAttackTable(u64 (&KnightAttack)[64]);
    void pawnAttackTable(u64 (&PawnAttack)[2][64]);
    void kingAttackTable(u64 (&KingAttack)[64]);
}
