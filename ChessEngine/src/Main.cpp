#include "Board.h";
using namespace ChessEngine;

int main() {
	Fen newGame("r1bqk2r/ppp1bppp/2np1n2/1B2p3/3PP3/2N2N2/PPP2PPP/R1BQK2R w KQkq - 2 6");
	std::unique_ptr<Board> board;
	board = std::make_unique<Board>(newGame);
	board->printBoard();
	Move ma(e1, g1, castling);
	board->doMove(ma);
	board->printBoard();
	board->undoMove(ma);
	board->printBoard();


}
