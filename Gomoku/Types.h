#pragma once
enum piece { none = 0, black = -1, white = 1};
enum turn { computer, human, noOne};
enum windowtoopenenum{ mainWindow, TwoPlayersWindow, GameWithComputerWindow, NoWindow, CloseProgram};
enum colorToPlay {black_to_play = -1, white_to_play = 1};
struct Move {
	int i;
	int j;
	int score;
};