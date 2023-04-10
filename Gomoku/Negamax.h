#pragma once
#include <vector>
#include <array>
#include <algorithm>
#include <map> 
#include <time.h>
#include <chrono>
#include <random>
#include <iostream>
#include "Types.h"
using namespace std;
class Negamax
{
public:	
	Negamax(int maxDepth);
	Move getMove(colorToPlay color, int Board[15][15]);
	
private:

	int fc = 0;

	struct CacheNode {
		int score;
		int depth;
		int Flag;
	};
	map<int, int> StateCache;
	map<int, CacheNode> Cache;
	int StateCachePuts = 0;	

	int CacheHits = 0;
	int CacheCutoffs = 0;
	int CachePuts = 0;
	int MaximumDepth = 3;
	int StateCacheHits = 0;

	Move bestMove;

	// live - открытый 
	// death - полуоткрытый
	static const int Rows = 15;
	static const int Columns = 15;

	const int WIN_DETECTED = INT_MIN;
	static const int LiveOne = 10;
	static const int DeadOne = 1;
	static const int LiveTwo = 100;
	static const int DeadTwo = 10;
	static const int LiveThree = 1000;
	static const int DeadThree = 100;
	static const int LiveFour = 10000;
	static const int DeadFour = 1000;
	static const int Five = 100000;

	int Table[Rows][Columns][2];	

	static bool move_sorter(Move const& move1, Move const& move2);
	int evaluateblock(int blocks, int pieces);
	int eval_board(int Board[Rows][Columns], int pieceType, array<int, 4> const& restrictions);
	array<array<int, 9>, 4> get_directions(int Board[Rows][Columns], int x, int y);
	bool check_directions(array<int, 9> const& arr);
	bool checkwin(int Board[Rows][Columns], int x, int y);
	bool remoteCell(int Board[Rows][Columns], int r, int c);
	array<int, 4> Get_restrictions(int Board[Rows][Columns]);
	array<int, 4> Change_restrictions(array<int, 4> const& restrictions, int i, int j);
	int get_seq(int y, int e);
	int evalff(int seq);
	int evaluate_state(int Board[Rows][Columns], int player, int hash, array<int, 4> const& restrictions);
	int evaluate_direction(array<int, 9> const& direction_arr, int player);
	int evalute_move(int Board[Rows][Columns], int x, int y, int player);
	vector<Move> BoardGenerator(array<int, 4> const& restrictions, int Board[15][15], int player);
	void Table_init();
	int hash_board(int board[Rows][Columns]);
	int update_hash(int hash, int player, int row, int col);
	int negamax(int newBoard[Rows][Columns], int player, int depth, int a, int b, int hash, array<int, 4> const& restrictions, int last_i, int last_j);
};

