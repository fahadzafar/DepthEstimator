/*
 * StereoParamXML.cpp
 *
 *  Created on: Jul 9, 2014
 *      Author: fahadz
 */

#include <string>
#include <sstream>
#include <stdlib.h>

#include <opencv/cv.h>

#include "Matrices.h"
#include "StereoParamXML.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

using namespace std;
using namespace rapidxml;
using namespace cv;

stringstream StereoParamXML::ss;
int StereoParamXML::debug = 0;

Matrix3 StereoParamXML::convertStringToM3x3(string data) {
	ss << data;
	Matrix3 parsedMat;

	for (int i = 0; i < 9; i++) {
		ss >> parsedMat[i];
	}
	return parsedMat;
}

Mat StereoParamXML::readMathNode(xml_node<>* math_node) {
	ss.clear();
	// Read the rows and columns.
	int rows = atoi(math_node->first_node("rows")->value());
	int cols = atoi(math_node->first_node("cols")->value());

	string data = math_node->first_node("data")->value();
	ss << data;

	// Create the appropriate matrix.
	Mat M(rows, cols, CV_64F);

	// Write the data correctly into the matrix.
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			double holder = 0;
			ss >> holder;
			M.at<double>(y, x) = holder;
		}
	}

	if (debug) {
		cout << "\n" << math_node->name() << "----- rows: " << rows
				<< ", cols: " << cols << ", :\n" << M << endl;
	}

	return M;
}

void StereoParamXML::loadXML(string filename) {

	rapidxml::file<> xmlFile(filename.c_str()); // Default template is char
	xml_document<> doc;

// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(xmlFile.data());

// Root node track.
	xml_node<>* root_node = doc.first_node("opencv_storage");

// Read the cameraLeft and cameraRight Matrices.
	string matrix3x3Data =
			root_node->first_node("cameraMatrixLeft")->first_node("data")->value();

	cameraMatrixLeft = readMathNode(root_node->first_node("cameraMatrixLeft"));
	distCoeffsLeft = readMathNode(root_node->first_node("distCoeffsLeft"));

	cameraMatrixRight = readMathNode(
			root_node->first_node("cameraMatrixRight"));
	distCoeffsRight = readMathNode(root_node->first_node("distCoeffsRight"));

// Read the rotation and translation matrices.
	relativeRotationLeftToRight = readMathNode(
			root_node->first_node("relativeRotationLeftToRight"));
	relativeTranslationLeftToRight = readMathNode(
			root_node->first_node("relativeTranslationLeftToRight"));

// Read the rectified matrices.
	rectificationRotationMatrixLeft = readMathNode(
			root_node->first_node("rectificationRotationMatrixLeft"));
	rectificationRotationMatrixRight = readMathNode(
			root_node->first_node("rectificationRotationMatrixRight"));

// Read the rectified projection matrices.
	rectifiedProjectionMatrixLeft = readMathNode(
			root_node->first_node("rectifiedProjectionMatrixLeft"));
	rectifiedProjectionMatrixRight = readMathNode(
			root_node->first_node("rectifiedProjectionMatrixRight"));
}
