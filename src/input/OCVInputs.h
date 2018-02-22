/*
 * Inputs.h
 *
 *  Created on: Jun 10, 2014
 *      Author: fahadz
 */

#ifndef INPUTS_H_
#define INPUTS_H_

#include "Matrices.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <string>

using namespace std;

#define myfloat float

class In {
public:
	// The total number of CPU compute threads used to compute
	// depth maps for a set of input images.
	static int NUM_THREADS;

	// The input must have 4 images per .raw file. Out of those
	// 4 images, 2/3/4 can be used for the depth map estimation based
	// on the speed vs. quality tradeoff since only 2 are required.
	// This parameter sets the number of images used for calculating
	// the depths maps.
	static int IMAGES_USED_FOR_DEPTH_CALCULATION;

	// The images input the the
	static int imageWidth;
	static int imageHeight;

	static cv::Mat planeNormal;

	//--------------------- Experiment Parameters.
	static float depthStartFrom;
	static float depthEndAt;
	static float depthIncrement;

	static float patchAreaNCC;

	static bool fix4ForDistortion;

	//------------------------.raw input frames reding indices.
	static int startRead;
	static int endRead;

	static int patchAreaMedian;

	//------------------------Data output directories
	static string depthMapDirGrey;
	static string depthMapDirBin;
	static string depthMapDirStitched;

	// The directory locations.
	static string pathInputRAWData;
	static string pathInputStereParamXMLData;

	static bool checkerboardTest;

	static double nccThreshold;
};

#endif /* INPUTS_H_ */
