/*
 * imageprocessor.cpp
 *
 *  Created on: 26 Aug 2017
 *      Author: karl
 */

using namespace std;

#include "imageprocessor.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/text/ocr.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace cv;
using namespace cv::text;

Mat getImageCannyBorders( Mat src);
vector<Point> findBordersPoints(Mat src, bool debug = false);
Mat fourPointsTransform(Mat src, vector<Point>);
vector<Point> orderPoints(vector<Point> points);

int image2int(Mat image)
{
    string vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // must have the same order as the classifier output classes

    Ptr<OCRHMMDecoder::ClassifierCallback> ocr = loadOCRHMMClassifierCNN("/home/karl/workspace/opencv_contrib/modules/text/samples/OCRBeamSearch_CNN_model_data.xml.gz");

    vector<int> out_classes;
    vector<double> out_confidences;

    ocr->eval(image, out_classes, out_confidences);

  /*  cout << "OCR output = \"" << vocabulary[out_classes[0]] << "\" with confidence "
         << out_confidences[0] << ". Evaluated in "
         << ((double)getTickCount() - t_r)*1000/getTickFrequency() << " ms." << endl << endl;
*/
    return ((int) vocabulary[out_classes[0]] - '0');
}

/** @function detectDigits */
vector<cv::Rect> detectDigits(cv::Mat img)
{
	vector<Rect> boundRect;
    Mat rgb;
    // downsample and use it for processing
    pyrDown(img, rgb);
    Mat small;
    cvtColor(rgb, small, CV_BGR2GRAY);
    // morphological gradient
    Mat grad;
    Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
    morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
    // binarize
    Mat bw;
    threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
    // connect horizontally oriented regions
    Mat connected;
    morphKernel = getStructuringElement(MORPH_RECT, Size(9, 1));
    morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(bw, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    for(int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
    	Rect cursor = boundingRect(contours[idx]);
    	if (((cursor.height < bw.rows / 9)) && ((cursor.width < bw.cols / 9))) // <-- avoid adding things bigger than a square
    	{
    		// add number to the result
			boundRect.push_back(cursor);
			// recover the original size
			boundRect[boundRect.size()-1].x = boundRect[boundRect.size()-1].x * 2;
			boundRect[boundRect.size()-1].y = boundRect[boundRect.size()-1].y * 2;
			boundRect[boundRect.size()-1].width = boundRect[boundRect.size()-1].width * 2;
			boundRect[boundRect.size()-1].height = boundRect[boundRect.size()-1].height * 2;
    	}
    }
    return boundRect;
}

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
	vector<Point> borders = findBordersPoints(rst, false);
	Mat result = fourPointsTransform(src, borders);
	if (debug)
	{
		char* window_name = "Sudoku";
		namedWindow( window_name, CV_WINDOW_AUTOSIZE );
		imshow( window_name, result);
		waitKey(0);
	}
	return result;
}

void printBoard(int node[9][9]) {
	cout << "\n";
	for (int x = 0; x < 9; x++)
	{
		for (int y = 0; y < 9; y++)
		{
			cout << node[x][y] << " ";
		}
		cout << "\n";
	}
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

Board image2board(char *image, bool debug = false)
{
	Board board;
	// initialize board
	for(int x=0;x<9;x++)
		 for(int y=0;y<9;y++)
			 board.node[x][y] = 0;
	/// Load an image
	Mat src = imread( image );
	if( !src.data )
		return board;
	// Identify board
	Mat imgboard = getBoard(src, false);
	// Identify numbers
	vector<Rect> boxNumber = detectDigits(imgboard);
	// Iterate each number, identifying their position and fill the board to return
	for (unsigned int i = 0; i < boxNumber.size(); i++)
	{
		Mat digit = imgboard(boxNumber[i]);
		int number = image2int(digit);
		int x = boxNumber[i].x / (imgboard.cols / 9);
		int y = boxNumber[i].y / (imgboard.rows / 9);
		board.node[y][x] = number;
	}
	if (debug) printBoard(board.node);
	return board;
}
