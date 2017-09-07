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
#include <iostream>

using namespace cv;
using namespace std;

Mat result;
bool debug = 0;

int Sudoku::choiseOptionsAt(int boardOptions[9][9][9], int x, int y) {
	int buff[9] = {1,2,3,4,5,6,7,8,9}, cursor;
	for (int i = 0; i < 9; i++)
	{
		// iterate cols
		if (i != x)
		{
			cursor = 0;
			while (boardOptions[i][y][cursor] > 0) {
				buff[boardOptions[i][y][cursor] - 1] = -1;
				cursor++;
			}
		}
		// iterate cols
		if (i != y)
		{
			cursor = 0;
			while (boardOptions[x][i][cursor] > 0) {
				buff[boardOptions[x][i][cursor] - 1] = -1;
				cursor++;
			}
		}
	}
	int trix = x / 3; // get small square position x
	int triy = y / 3; // get small square position y
	// iterate each element of the small squere
	for (int tx = trix * 3; tx < trix * 3 + 3; tx++)
		for (int ty = triy * 3; ty < triy * 3 + 3; ty++)
		{
			if ((tx != x) && (ty != y))
			{
				cursor = 0;
				while (boardOptions[tx][ty][cursor] > 0) {
					buff[boardOptions[tx][ty][cursor] - 1] = -1;
					cursor++;
				}
			}
		}
	// search response
	for (int cursor : buff)
	{
		if (cursor > 0)
		{
			return cursor;
		}
	}

	return -1;
}

bool Sudoku::numberInBox(int node[9][9], int number, int posX, int posY) {
	int trix = posX / 3; // get small square position x
	int triy = posY / 3; // get small square position y
	// iterate each element of the small squere
	for (int x = trix * 3; x < trix * 3 + 3; x++)
		for (int y = triy * 3; y < triy * 3 + 3; y++)
		{
			if ( !((posX == x) && (posY == y))
			    && (node[x][y] == number))
			{
				return true;
			}
		}

	return false;
}

bool Sudoku::numberInCrux(int node[9][9], int number, int posX, int posY) {
	for (int x = 0; x < 9; x++)
	{
		if ((x != posX) && (node[x][posY] == number))
			return true;
	}
	for (int y = 0; y < 9; y++)
	{
		if ((y != posY) && (node[posX][y] == number))
			return true;
	}
	return false;
}

void Sudoku::getOptionsFor(Board board, int x, int y, int result[9]) {
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

int Sudoku::process()
{
	int empty = 0;
	// fill the options per node
	int options[9][9][9];
	// int vec[9];
	for(int number = 1; number < 10; number++) {
		// make the option matrix
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
			{
				/* if (board.node[x][y] == number)
				{
					options[number-1][x][y] = number;
				} else */
				if ((board.node[x][y] == 0) && (!numberInBox(board.node, number, x, y) && (!numberInCrux(board.node, number, x, y))))
				{
					options[number-1][x][y] = number;
				} else {
					options[number-1][x][y] = 0;
				}
			}
	}
	// first filter to the matrix
	for(int number = 1; number < 10; number++) {
		for (int x = 0; x < 9; x++)
			for (int y = 0; y < 9; y++)
			{
				if (options[number-1][x][y] != 0)
				{
					// check if this number is the unique in the option box
					if (!numberInBox(options[number-1], number, x, y))
					{
						board.node[x][y] = number;
					} else {
						empty++;
					}
				}
			}
	}

	return empty;
}

int Sudoku::resolve(char *img) {
	bool debug = false;
	board = image2board(img, debug);
	int emptySpaces = 9;
	while (emptySpaces > 0)
	{
		emptySpaces = process();
		if (debug)
		{
			printBoard(board.node);
			cout << "\n" << emptySpaces << "\n";
		}
	}
	printBoard(board.node);
	return 0;
}
