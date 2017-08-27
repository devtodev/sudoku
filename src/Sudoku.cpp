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

Sudoku::Sudoku() {
	// TODO Auto-generated constructor stub

}

Sudoku::~Sudoku() {
}

void Sudoku::getBoard() {
	//	Mat result = getImageCannyBorders(source);
	//	vector<Point> borders = findBordersPoints(result, debug);
	//	return fourPointsTransform(src, borders);
}

int Sudoku::show() {
	char* window_name = "Sudoku";
	namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	imshow( window_name, result);
	waitKey(0);
	return 0;

}

int Sudoku::resolve(char *img) {
	board = image2board(img);
	return 0;
}
