/*
 * SATImage.cpp
 *
 *  Created on: Jul 10, 2014
 *      Author: fahadz
 */

#include "OCVInputs.h"
#include <SATImage.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

using namespace std;
using namespace cv;

SATImage::SATImage(Mat& inputImg) {
	height = inputImg.rows;
	width = inputImg.cols;

	SATData = (double*) calloc(width * height, sizeof(double));

	// Set the first element.
	SATData[0] = inputImg.at<double>(0, 0);

	// Setup first the row.
	for (int i = 1; i < width; i++) {
		SATData[i] = inputImg.at<double>(i, 0) + SATData[i - 1];
	}

	// Setup the first column.
	for (int i = 1; i < height; i++) {
		SATData[i * width + 0] = inputImg.at<double>(0, i)
				+ SATData[(i - 1) * width];
	}

	// Run the SAT on all the image.
	for (int i = 1; i < height; i++) {
		for (int j = 1; j < width; j++) {
			SATData[i * width + j] = inputImg.at<double>(j, i)
					+ SATData[(i - 1) * width + j] + SATData[i * width + j - 1]
					- SATData[(i - 1) * width + (j - 1)];
		}
	}

	double *extraData = (double*) calloc((width + 1) * (height + 1),
			sizeof(double));

	// Run the SAT on all the image.
	for (int i = 1; i < height + 1; i++) {
		for (int j = 1; j < width + 1; j++) {
			extraData[i * (width + 1) + j] = SATData[(i - 1) * width + (j - 1)];
		}
	}
	// The width and height of the SAT iamge is +1 on both dimensions
	// Its for easy lookup at the edges.
	height = height + 1;
	width = width + 1;
	delete SATData;
	SATData = extraData;
}

double SATImage::EvaluateSAT(int x, int y, int x2, int y2) {
	x2 = x2 + 1;
	y2 = y2 + 1;

	x = x + 1;
	y = y + 1;

	int w = In::imageWidth + 1;
	double sum = SATData[y2 * w + x2] - SATData[(y - 1) * w + x2]
			- SATData[y2 * w + x - 1] + SATData[(y - 1) * w + x - 1];
	return sum;
}

SATImage::~SATImage() {
	delete SATData;
}

