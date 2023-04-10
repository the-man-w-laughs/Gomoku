#include "gomoku.h"

Gomoku::Gomoku(int board_length)
	: board_length_{ board_length },
	turn_{ black },
	last_move_{ -1,-1 }
{		
	for (int i = 0; i < board_length_; i++) {
		for (int j = 0; j < board_length_; j++) {
			board[i][j] = none;
		}
	}
}

bool Gomoku::PieceOnBoard(int x, int y)
{
	//check empty and set
	if (board[x][y] == none) {
		board[x][y] = turn_;

		//record last move to CheckWin()
		last_move_ = { x,y };

		return true;
	}
	return false;
}


bool Gomoku::CheckWin() {
	int x{ last_move_.x };
	int y{ last_move_.y };
	int connected_piece{ 1 };

	//check vertical 
	for (int i = 1; getPiece(x, y - i) == turn_; i++)
		connected_piece++;
	for (int i = 1; getPiece(x, y + i) == turn_; i++)
		connected_piece++;

	if (connected_piece >= 5)
		return true;

	//check diagonal right up
	connected_piece = 1;
	for (int i = 1; getPiece(x + i, y - i) == turn_; i++)
		connected_piece++;
	for (int i = 1; getPiece(x - i, y + i) == turn_; i++)
		connected_piece++;

	if (connected_piece >= 5)
		return true;

	//check diagonal left up
	connected_piece = 1;
	for (int i = 1; getPiece(x - i, y - i) == turn_; i++)
		connected_piece++;
	for (int i = 1; getPiece(x + i, y + i) == turn_; i++)
		connected_piece++;

	if (connected_piece >= 5)
		return true;

	//check horizontal
	connected_piece = 1;
	for (int i = 1; getPiece(x + i, y) == turn_; i++)
		connected_piece++;
	for (int i = 1; getPiece(x - i, y) == turn_; i++)
		connected_piece++;

	if (connected_piece >= 5)
		return true;

	return false;
}

void Gomoku::reset() {
	for (int i = 0; i < board_length_; i++)
		for (int j = 0; j < board_length_; j++)
			board[i][j] = none;

	turn_ = black;
}
