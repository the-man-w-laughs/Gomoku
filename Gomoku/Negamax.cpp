#include "Negamax.h"
Negamax::Negamax(int maxDepth) {
    MaximumDepth = maxDepth;
    //Table_init();
}


Move Negamax::getMove(colorToPlay color, int Board[15][15])
{
    int depth = MaximumDepth;
    Table_init();
    int res = negamax(Board, color, depth, INT_MIN, INT_MAX, hash_board(Board) - 1, Get_restrictions(Board), 0, 0);
    Cache.clear();
    StateCache.clear();

    return bestMove;
}

bool Negamax::move_sorter(Move const& move1, Move const& move2) {
	return move1.score > move2.score;
}

int Negamax::evaluateblock(int blocks, int pieces) {
	if (blocks == 0) {
		switch (pieces) {
		case 1:
			return LiveOne;
		case 2:
			return LiveTwo;
		case 3:
			return LiveThree;
		case 4:
			return LiveFour;
		default:
			return Five;
		}
	}
	else if (blocks == 1) {
		switch (pieces) {
		case 1:
			return DeadOne;
		case 2:
			return DeadTwo;
		case 3:
			return DeadThree;
		case 4:
			return DeadFour;
		default:
			return Five;
		}
	}
	else {
		if (pieces >= 5) {
			return Five;
		}
		else {
			return 0;
		}
	}
}

int Negamax::eval_board(int Board[Rows][Columns], int pieceType, array<int, 4> const& restrictions) {
	int score = 0;
	int min_r = restrictions[0];
	int min_c = restrictions[1];
	int max_r = restrictions[2];
	int max_c = restrictions[3];
	for (int row = min_r; row < max_r + 1; row++) {
		for (int column = min_c; column < max_c + 1; column++) {
			if (Board[row][column] == pieceType) {
				int block = 0;
				int piece = 1;
				// left
				if (column == 0 || Board[row][column - 1] != 0) {
					block++;
				}
				// pieceNum
				for (column++; column < Columns && Board[row][column] == pieceType; column++) {
					piece++;
				}
				// right
				if (column == Columns || Board[row][column] != 0) {
					block++;
				}
				score = score + evaluateblock(block, piece);
			}
		}
	}

	for (int column = min_c; column < max_c + 1; column++) {
		for (int row = min_r; row < max_r + 1; row++) {
			if (Board[row][column] == pieceType) {
				int block = 0;
				int piece = 1;
				// left
				if (row == 0 || Board[row - 1][column] != 0) {
					block++;
				}
				// pieceNum
				for (row++; row < Rows && Board[row][column] == pieceType; row++) {
					piece++;
				}
				// right
				if (row == Rows || Board[row][column] != 0) {
					block++;
				}
				score += evaluateblock(block, piece);
			}
		}
	}

	for (int n = min_r; n < (max_c - min_c + max_r); n += 1) {
		int r = n;
		int c = min_c;
		while (r >= min_r && c <= max_c) {
			if (r <= max_r) {
				if (Board[r][c] == pieceType) {
					int block = 0;
					int piece = 1;
					// left
					if (c == 0 || r == Rows - 1 || Board[r + 1][c - 1] != 0) {
						block++;
					}
					// pieceNum
					r--;
					c++;
					for (; r >= 0 && Board[r][c] == pieceType; r--) {
						piece++;
						c++;
					}
					// right
					if (r < 0 || c == Columns || Board[r][c] != 0) {
						block++;
					}
					score += evaluateblock(block, piece);
				}
			}
			r -= 1;
			c += 1;
		}
	}

	for (int n = min_r - (max_c - min_c); n <= max_r; n++) {
		int r = n;
		int c = min_c;
		while (r <= max_r && c <= max_c) {
			if (r >= min_r && r <= max_r) {
				if (Board[r][c] == pieceType) {
					int  block = 0;
					int piece = 1;
					// left
					if (c == 0 || r == 0 || Board[r - 1][c - 1] != 0) {
						block++;
					}
					// pieceNum
					r++;
					c++;
					for (; r < Rows && Board[r][c] == pieceType; r++) {
						piece++;
						c++;
					}
					// right
					if (r == Rows || c == Columns || Board[r][c] != 0) {
						block++;
					}
					score += evaluateblock(block, piece);
				}
			}
			r += 1;
			c += 1;
		}

	}
	return score;
}

// return array[][] containing values of board cells for each of 4 direction
// for specific cell (x,y)
array<array<int, 9>, 4> Negamax::get_directions(int Board[15][15], int x, int y) {
	array<int, 9> a;
	array<int, 9> b;
	array<int, 9> c;
	array<int, 9> d;
	int a_i = 0;
	int b_i = 0;
	int c_i = 0;
	int d_i = 0;

	// enumeration through all near cells
	for (int i = -4; i < 5; i++) {
		// horizontal
		if (x + i >= 0 && x + i <= Rows - 1) {
			a[a_i] = Board[x + i][y];
			a_i++;
			// diagonal
			if (y + i >= 0 && y + i <= Columns - 1) {
				b[b_i] = Board[x + i][y + i];
				b_i++;
			}
		}
		// vertical
		if (y + i >= 0 && y + i <= Columns - 1) {
			c[c_i] = Board[x][y + i];
			c_i++;
			// diagonal
			if (x - i >= 0 && x - i <= Rows - 1) {
				d[d_i] = Board[x - i][y + i];
				d_i++;
			}
		}

	}
	// end of an array
	if (a_i != 9) {
		a[a_i] = 2;
	}
	if (b_i != 9) {
		b[b_i] = 2;
	}
	if (c_i != 9) {
		c[c_i] = 2;
	}
	if (d_i != 9) {
		d[d_i] = 2;
	}

	array<array<int, 9>, 4> Directions = { a,b,c,d };
	return Directions;
}

// check one direction
bool Negamax::check_directions(array<int, 9> const& arr) {
	int size = 9;
	for (int i = 0; i < size - 4; i++) {
		if (arr[i] != 0) {
			// to prevent extra iterations
			if (arr[i] == 2 || arr[i + 1] == 2 || arr[i + 2] == 2 || arr[i + 3] == 2 || arr[i + 4] == 2) {
				return false;
			}
			if (arr[i] == arr[i + 1] && arr[i] == arr[i + 2] && arr[i] == arr[i + 3] && arr[i] == arr[i + 4]) {
				return true;
			}
		}
	}
	return false;
}

// check all directions
bool Negamax::checkwin(int Board[15][15], int x, int y) {
	array<array<int, 9>, 4> Directions = get_directions(Board, x, y);
	for (int i = 0; i < 4; i++) {

		if (check_directions(Directions[i])) {
			return true;
		}
	}
	return false;
}

bool Negamax::remoteCell(int Board[15][15], int r, int c) {
    for (int i = r - 2; i <= r + 2; i++) {
        if (i < 0 || i >= Rows) continue;
        for (int j = c - 2; j <= c + 2; j++) {
            if (j < 0 || j >= Columns) continue;
            if (Board[i][j] != 0) return false;
        }
    }
    return true;
}
array<int, 4> Negamax::Get_restrictions(int Board[15][15]) {
    int min_r = INT_MAX;
    int min_c = INT_MAX;
    int max_r = INT_MIN;
    int max_c = INT_MIN;
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Columns; j++) {
            if (Board[i][j] != 0) {
                min_r = min(min_r, i);
                min_c = min(min_c, j);
                max_r = max(max_r, i);
                max_c = max(max_c, j);
            }
        }
    }
    if (min_r - 2 < 0) {
        min_r = 2;
    }
    if (min_c - 2 < 0) {
        min_c = 2;
    }
    if (max_r + 2 >= Rows) {
        max_r = Rows - 3;
    }
    if (max_c + 2 >= Columns) {
        max_c = Columns - 3;
    }
    array<int, 4> restrictions = { min_r, min_c, max_r, max_c };
    return restrictions;
}

array<int, 4> Negamax::Change_restrictions(array<int, 4> const& restrictions, int i, int j) {
    int min_r = restrictions[0];
    int min_c = restrictions[1];
    int max_r = restrictions[2];
    int max_c = restrictions[3];
    if (i < min_r) {
        min_r = i;
    }
    else if (i > max_r) {
        max_r = i;
    }
    if (j < min_c) {
        min_c = j;
    }
    else if (j > max_c) {
        max_c = j;
    }
    if (min_r - 2 < 0) {
        min_r = 2;
    }
    if (min_c - 2 < 0) {
        min_c = 2;
    }
    if (max_r + 2 >= Rows) {
        max_r = Rows - 3;
    }
    if (max_c + 2 >= Columns) {
        max_c = Columns - 3;
    }
    array<int, 4> new_restrictions = { min_r, min_c, max_r, max_c };
    return new_restrictions;
}

int Negamax::get_seq(int y, int e) {
    if (y + e == 0) {
        return 0;
    }
    if (y != 0 && e == 0) {
        return y;
    }
    if (y == 0 && e != 0) {
        return -e;
    }
    if (y != 0 && e != 0) {
        return 17;
    }
}

int Negamax::evalff(int seq) {
    switch (seq) {
    case 0:
        return 7;
    case 1:
        return 35;
    case 2:
        return 800;
    case 3:
        return 15000;
    case 4:
        return 800000;
    case -1:
        return 15;
    case -2:
        return 400;
    case -3:
        return 1800;
    case -4:
        return 100000;
    case 17:
        return 0;
    }
}
int Negamax::evaluate_state(int Board[15][15], int player, int hash, array<int, 4> const& restrictions) {
    int black_score = eval_board(Board, -1, restrictions);
    int white_score = eval_board(Board, 1, restrictions);
    int score = 0;
    if (player == -1) {
        score = (black_score - white_score);
    }
    else {
        score = (white_score - black_score);
    }
    StateCache[hash] = score;
    StateCachePuts++;
    return score;
}


int Negamax::evaluate_direction(array<int, 9> const& direction_arr, int player) {
    int score = 0;
    int arr_size = direction_arr.size();
    for (int i = 0; (i + 4) < arr_size; i++) {
        int you = 0;
        int enemy = 0;
        if (direction_arr[i] == 2) {
            return score;
        }
        for (int j = 0; j <= 4; j++) {
            if (direction_arr[i + j] == 2) {
                return score;
            }
            if (direction_arr[i + j] == player) {
                you++;
            }
            else if (direction_arr[i + j] == -player) {
                enemy++;
            }
        }
        score += evalff(get_seq(you, enemy));
        if ((score >= 800000)) {
            return WIN_DETECTED;
        }
    }
    return score;
}



int Negamax::evalute_move(int Board[15][15], int x, int y, int player) {
    int score = 0;
    array<array<int, 9>, 4> Directions = get_directions(Board, x, y);
    int temp_score;
    for (int i = 0; i < 4; i++) {
        temp_score = evaluate_direction(Directions[i], player);
        if (temp_score == WIN_DETECTED) {
            return WIN_DETECTED;
        }
        else {
            score += temp_score;
        }
    }
    return score;
}

vector<Move> Negamax::BoardGenerator(array<int, 4> const& restrictions, int Board[15][15], int player) {
    vector<Move> availSpots_score; //c is j  r is i;
    int min_r = restrictions[0];
    int min_c = restrictions[1];
    int max_r = restrictions[2];
    int max_c = restrictions[3];
    for (int i = min_r - 2; i <= max_r + 2; i++) {
        for (int j = min_c - 2; j <= max_c + 2; j++) {
            if (Board[i][j] == 0 && !remoteCell(Board, i, j)) {
                Move move;
                move.i = i;
                move.j = j;
                move.score = evalute_move(Board, i, j, player);
                if (move.score == WIN_DETECTED) {
                    vector<Move> winning_move = { move };
                    return winning_move;
                }
                availSpots_score.push_back(move);
            }
        }
    }
    sort(availSpots_score.begin(), availSpots_score.end(), &move_sorter);
    //  return availSpots_score.slice(0,20)
    return availSpots_score;
}

void Negamax::Table_init() {
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Columns; j++) {
            Table[i][j][0] = rand(); //1
            Table[i][j][1] = rand(); //2
        }
    }
}

int Negamax::hash_board(int board[15][15]) {
    int h = 0;
    int p;
    for (int i = 0; i < Rows; i++) {
        for (int j = 0; j < Columns; j++) {
            int Board_value = board[i][j];
            if (Board_value != 0) {
                if (Board_value == -1) {
                    p = 0;
                }
                else {
                    p = 1;
                }
                h = h ^ Table[i][j][p];
            }
        }
    }
    return h;
}

int Negamax::update_hash(int hash, int player, int row, int col) {
    if (player == -1) {
        player = 0;
    }
    else {
        player = 1;
    }
    hash = hash ^ Table[row][col][player];
    return hash;
}

int Negamax::negamax(int newBoard[15][15], int player, int depth, int a, int b, int hash, array<int, 4> const& restrictions, int last_i, int last_j) {
    const int alphaOrig = a;
    if ((Cache.count(hash)) && (Cache[hash].depth >= depth)) { //if exists
        CacheHits++;
        int score = Cache[hash].score;
        if (Cache[hash].Flag == 0) {
            CacheCutoffs++;
            return score;
        }
        if (Cache[hash].Flag == -1) {
            a = max(a, score);
        }
        else if (Cache[hash].Flag == 1) {
            b = min(b, score);
        }
        if (a >= b) {
            CacheCutoffs++;
            return score;
        }
    }

    fc++;
    if (checkwin(newBoard, last_i, last_j)) {
        return -2000000 + (MaximumDepth - depth);
    }

    if (depth == 0) {
        if (StateCache.count(hash)) { //if exists
            StateCacheHits++;
            return StateCache[hash];
        }
        return evaluate_state(newBoard, player, hash, restrictions);
    }
    vector<Move> availSpots = BoardGenerator(restrictions, newBoard, player);

    int availSpots_size = availSpots.size();
    if (availSpots_size == 0) {
        return 0;
    }


    int i, j;
    int newHash;
    int bestvalue = INT_MIN;
    int value;
    for (int y = 0; y < availSpots_size; y++) {
        i = availSpots[y].i;
        j = availSpots[y].j;

        newHash = update_hash(hash, player, i, j);
        newBoard[i][j] = player;
        array<int, 4> new_restrictions = Change_restrictions(restrictions, i, j);
        value = -negamax(newBoard, -player, depth - 1, -b, -a, newHash, new_restrictions, i, j);
        newBoard[i][j] = 0;
        if (value > bestvalue) {
            bestvalue = value;
            if (depth == MaximumDepth) {
                bestMove = { i,j,value };
                //cout << "best move" << endl;
                //cout << bestMove.i << " " << bestMove.j << " " << bestMove.score << endl;
            }
        }
        a = max(a, value);
        if (a >= b) {
            break;
        }
    }
    //availSpots.clear();
    CachePuts++;
    CacheNode cache_node;

    cache_node.score = bestvalue;
    cache_node.depth = depth;
    if (bestvalue <= alphaOrig) {
        cache_node.Flag = 1;
    }
    else if (bestvalue >= b) {
        cache_node.Flag = -1;
    }
    else {
        cache_node.Flag = 0;
    }
    Cache[hash] = cache_node;
    return bestvalue;
}





