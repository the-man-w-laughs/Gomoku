#pragma once

#include <vector>
#include "Types.h"
using namespace std;

// move
struct Point_ {
	int x;
	int y;
};

// main class
class Gomoku {
public:	
	Gomoku(int board_length = 15);


	//order
	//PieceOnBoard -> CheckWin -> PassTurn

	// return true when success
	bool PieceOnBoard(int x, int y);

	//Check base on last move
	bool CheckWin();

	//pass turn
	void PassTurn() {
		if (turn_ == -1)
			turn_ = 1;
		else
			turn_ = -1;
	}

	void reset();

	// whose turn is
	int getturn() const { return turn_; }

	int board_length() const { return board_length_; }

	int board[15][15];
private:	
	int getPiece(int x, int y) {
		if (x < 0 || board_length_ <= x ||
			y < 0 || board_length_ <= y) {
			return none;
		}
		return board[x][y];
	}
	int turn_;
	int board_length_;
	

	Point_ last_move_;
};