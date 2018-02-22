// This class creates a Summed Area Table for an input image.

#ifndef SATIMAGE_H_
#define SATIMAGE_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

using namespace cv;

class SATImage {

public:
	// Holds the data array.
	double* SATData;

	// Dimension parameters for the summed area table image.
	int width;
	int height;

	// Constructor, uses the input matrix to calculate the
	// SAT when the constructor is called.
	SATImage(Mat& inputImg);

	// Returns the sum of the input block corner values of the image.
	double EvaluateSAT(int x, int y, int x2, int y2);

	// Used to de-allocate the double heap memory.
	virtual ~SATImage();
};

#endif /* SATIMAGE_H_ */
