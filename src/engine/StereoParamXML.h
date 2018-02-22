/*
 * StereoParamXML.h
 *
 *  Created on: Jul 9, 2014
 *      Author: fahadz
 */

#ifndef STEREOPARAMXML_H_
#define STEREOPARAMXML_H_

#include <string>
#include <sstream>

#include <opencv/cv.h>
#include "rapidxml.hpp"

using namespace std;
using namespace cv;
using namespace rapidxml;

class StereoParamXML {
	static stringstream ss;

public:
	static int debug;
	Mat cameraMatrixLeft;
	Mat distCoeffsLeft;
	Mat cameraMatrixRight;
	Mat distCoeffsRight;
	Mat relativeRotationLeftToRight;
	Mat relativeTranslationLeftToRight;

	Mat rectificationRotationMatrixLeft;
	Mat rectificationRotationMatrixRight;

	Mat rectifiedProjectionMatrixLeft;
	Mat rectifiedProjectionMatrixRight;

	// Loads the data into the Mat variables using the
	// Rapid XML parser.
	void loadXML(string data);

	// Converts a string stream to 3x3Matrix
	Matrix3 convertStringToM3x3(string);

	// Read a node that contains "row" and "col" into an openCV Mat structure.
	Mat readMathNode(xml_node<>* math_node);

};

#endif /* STEREOPARAMXML_H_ */
