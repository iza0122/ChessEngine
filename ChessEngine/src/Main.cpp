#include "Game.h";
using namespace ChessEngine;

int main() {
	Fen newGame("r2r1k2/ppR2Qp1/1q2pp1p/3p4/8/3P4/PP3PPP/2R3K1 b - - 1 24");
	Board myGame(newGame);
	myGame.printBoard();

}
