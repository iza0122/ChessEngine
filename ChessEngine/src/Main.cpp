#include "Game.h";
using namespace ChessEngine;

int main() {
	Game a;
	if (a.LoadGame("aaa.txt", "FEN")) {
		a.board.printBoard();
	}
	
}
