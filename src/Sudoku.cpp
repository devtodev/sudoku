/*
 * sudoku.cpp
 *
 *  Created on: 26 Aug 2017
 *      Author: karl
 */


#include "Sudoku.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


using namespace cv;
using namespace std;

Mat result;
bool debug = 0;

int Sudoku::choiseOptionsAt(int boardOptions[9][9][9], int x, int y) {
	return -1;
}

void Sudoku::getOptionsOf(Board board, int x, int y, int result[9]) {
	int temp[] = {1,2,3,4,5,6,7,8,9}; // <-- all the possibilities
	for (int i = 0; i < 9; i++)
	{
		// iterate cols
		if (board.node[y][i] != 0)
			temp[board.node[y][i] - 1] = -1;
		// iterate rows
		if (board.node[i][x] != 0)
			temp[board.node[i][x] - 1] = -1;
	}
	int trix = x / 3; // get small square position x
	int triy = y / 3; // get small square position y
	// iterate each element of the small squere
	for (int tx = trix * 3; tx < trix * 3 + 3; tx++)
		for (int ty = triy * 3; ty < triy * 3 + 3; ty++)
		{
			if (board.node[tx][ty] != 0)
				temp[board.node[tx][ty] - 1] = -1;
		}
	// prepare response
	int i = 0;
	for (int cursor : temp)
	{
		if (cursor > 0)
		{
			result[i] = cursor;
			i++;
		}
	}
	// complete empty positions with -1
	for (int x = i; x < 9; x++)
		result[x] = -1;
}

Sudoku::Sudoku() {
	// TODO Auto-generated constructor stub

}

Sudoku::~Sudoku() {
}

int Sudoku::show() {
	char* window_name = "Sudoku";
	namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	imshow( window_name, result);
	waitKey(0);
	return 0;
}

int Sudoku::resolve(char *img) {
	board = image2board(img, true);
	// fill the options per node
	int options[9][9][9];
	int vec[9];
	for (int x = 0; x < 9; x++) {
		for (int y = 0; y < 9; y++)
		{
			if (board.node[x][y] == 0)
			{
				getOptionsOf(board, x, y, vec);
				for (int cursor = 0; cursor < 9; cursor++)
				{
					options[x][y][cursor] = vec[cursor];
				}
			} else {
				options[x][y][0] = board.node[x][y];
				options[x][y][1] = -1;
			}
		}
	}
	for (int x = 0; x < 9; x++)
		for (int y = 0; y < 9; y++)
		{
			if (board.node[x][y] == 0)
			{
				board.node[x][y] = choiseOptionsAt(options, x, y);
			}
		}
	printBoard(board.node);
	return 0;
}
