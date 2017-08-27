/*
 * imageprocessor.cpp
 *
 *  Created on: 26 Aug 2017
 *      Author: karl
 */

#include "imageprocessor.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

Mat getImageCannyBorders( Mat src);
vector<Point> findBordersPoints(Mat src, bool debug = false);
Mat fourPointsTransform(Mat src, vector<Point>);
vector<Point> orderPoints(vector<Point> points);

/** @function getBorders */
Mat getImageCannyBorders( Mat src)
{
	Mat border;
	/// Convert it to gray
	cvtColor( src, border, CV_BGR2GRAY );
	GaussianBlur( border, border, Size(5,5), 0, 0, BORDER_DEFAULT );
	// Canny edge detector
	Canny(border, border, 75, 200);
	return border;
}

vector<Point> findBordersPoints(Mat src, bool debug)
{
	vector<vector<Point> > contours;	/// Find contours
	findContours( src, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	vector<vector<Point> > contours_poly( contours.size() );
	int selected = -1;
	for(unsigned int i = 0; i < contours.size(); i++ )
	{
		double peri = arcLength(contours[i], true);
		approxPolyDP( Mat(contours[i]), contours_poly[i], 0.02 * peri, true );
		if ((contours_poly[i].size() == 4))
		{
			if (debug)
			{
				contours_poly[i] = orderPoints(contours_poly[i]);
				rectangle(src,contours_poly[i][0],contours_poly[i][3],CV_RGB(255,255,255),5,8);
				char* window_name = "Sudoku";
				namedWindow( window_name, CV_WINDOW_AUTOSIZE );
				imshow( window_name, src);
				waitKey(0);
			}
			if (selected == -1) selected = i;
		}
	}

	return contours_poly[selected];
}

vector<Point> orderPoints(vector<Point> points)
{
	vector<Point> order;
	Point tl(std::numeric_limits<int>::max(), 0);
	Point tr(0,std::numeric_limits<int>::max());
	Point bl(std::numeric_limits<int>::max(),std::numeric_limits<int>::max());
	Point br(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

	for (int i = 0; i < 4;i++)
	{
		int x = points[i].x;
		int y = points[i].y;
		if (x + y < tl.x + tl.y) tl = points[i];
		if (x - y > tr.x - tr.y) tr = points[i];
		if (x - y < bl.x - bl.y) bl = points[i];
		if (x + y > br.x + br.y) br = points[i];
	}
	order.push_back(tl);
	order.push_back(tr);
	order.push_back(bl);
	order.push_back(br);
	return order;
}

Mat fourPointsTransform(Mat src, vector<Point> corners)
{
	Point2f points[4];
	vector<Point> pv = orderPoints(corners);
	std::copy(pv.begin(), pv.end(), points);
	Point tl = points[0];
	Point tr = points[1];
	Point bl = points[2];
	Point br = points[3];

	// compute the width of the new image, which will be the
	// maximum distance between bottom-right and bottom-left
	// x-coordiates or the top-right and top-left x-coordinates
	float widthA, widthB, maxWidth;
	float heightA, heightB, maxHeight;

	widthA = sqrt((pow((br.x - bl.x), 2)) + (pow((br.y - bl.y), 2)));
	widthB = sqrt((pow((tr.x - tl.x), 2)) + (pow((tr.y - tl.y), 2)));
	maxWidth = max(int(widthA), int(widthB));

	// compute the height of the new image, which will be the
	// maximum distance between the top-right and bottom-right
	// y-coordinates or the top-left and bottom-left y-coordinates
	heightA = sqrt((pow((tr.x - br.x), 2)) + (pow((tr.y - br.y), 2)));
	heightB = sqrt((pow((tl.x - bl.x), 2)) + (pow((tl.y - bl.y), 2)));
	maxHeight = max(int(heightA), int(heightB));

	// now that we have the dimensions of the new image, construct
	// the set of destination points to obtain a "birds eye view",
	// (i.e. top-down view) of the image, again specifying points
	// in the top-left, top-right, bottom-right, and bottom-left order
	Point2f dts[4];
	dts[0] = Point(0,0);
	dts[1] = Point(maxWidth-1,0);
	dts[2] = Point(0, maxHeight-1);
	dts[3] = Point(maxWidth-1,maxHeight-1);

	Mat warpMatrix = getPerspectiveTransform(points, dts);

	Mat rotated;
	Size size(maxWidth, maxHeight);
	warpPerspective(src, rotated, warpMatrix, size, INTER_LINEAR, BORDER_CONSTANT);
	return rotated;
}

Mat getBoard(Mat src, bool debug)
{
	Mat rst = getImageCannyBorders(src);
	vector<Point> borders = findBordersPoints(rst, debug);
	return fourPointsTransform(src, borders);
}

void drawLine(Vec2f line, Mat &img, Scalar rgb = CV_RGB(0,0,255))
{
    if(line[1]!=0)
    {
        float m = -1/tan(line[1]);

        float c = line[0]/sin(line[1]);

        cv::line(img, Point(0, c), Point(img.size().width, m*img.size().width+c), rgb);
    }
    else
    {
        cv::line(img, Point(line[0], 0), Point(line[0], img.size().height), rgb);
    }

}


Board image2board(char *image)
{
	Board board;
	for(int x=0;x<9;x++)
		 for(int y=0;y<9;y++)
			 board.node[x][y] = 0;
	/// Load an image
	Mat src = imread( image );
	if( !src.data )
		return board;

	// Process image
	Mat imgboard = getBoard(src, false);
	getBoard(imgboard, true);
	// TODO: continue here...

	return board;
}
