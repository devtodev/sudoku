/*
 * sudoku.h
 *
 *  Created on: 26 Aug 2017
 *      Author: karl
 */

#ifndef SRC_SUDOKU_H_
#define SRC_SUDOKU_H_

#include "imageprocessor.h"

class Sudoku {
	void getOptionsFor(Board board, int x, int y, int result[9]);
	int choiseOptionsAt(int boardOptions[9][9][9], int x, int y);
public:
	Board board;
	Sudoku();
	int resolve(char *img);
	int show();
	virtual ~Sudoku();
};

#endif /* SRC_SUDOKU_H_ */
