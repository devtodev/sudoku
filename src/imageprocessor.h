/*
 * imageprocessor.h
 *
 *  Created on: 26 Aug 2017
 *      Author: karl
 */

#ifndef SRC_IMAGEPROCESSOR_H_
#define SRC_IMAGEPROCESSOR_H_


struct Board {
	int node[9][9];
};

Board image2board(char *image);


#endif /* SRC_IMAGEPROCESSOR_H_ */
