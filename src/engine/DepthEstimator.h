// This class is the core manager for calculating a depth map from a set of stereo images.
// It reads the projection, rotation, translation and distortion parameters from the input XML files
// then stores them into static variables which can be shared from thread safe lookup.
// The execution threads call the runDepthEstimationEngine to simply compute the depth map
// for a set of frames taken at the same time.

#ifndef DEPTHESTIMATOR_H_
#define DEPTHESTIMATOR_H_

#include <vector>

#include "OCVInputs.h"
#include "Matrices.h"
#include "rapidxml.hpp"

#include <StereoParamXML.h>
#include <opencv/cv.h>
#include <string>
#include <vector>

#include "SATImage.h"

using namespace cv;

class DepthEstimator {
private:
	static Mat K[4];
	static Mat trans[4];
	static Mat rot[4];
	static StereoParamXML K_0_1_file, K_0_2_file, K_2_3_file, K_3_1_file;
	static Mat distCoeffs[4];
	static string rawDataDir;

	// The complete 4 images stacked in the x-axis.
	// (width = 1600, height = 400) where each image is 400x400.
	Mat iImages[4];

	// Stores the complete 4-set image.
	Mat imageAllCameras;

	// Gets the homography that maps the points from the old camera to the new camera.
	Mat getHomorgrapy(Mat rot, Mat trans, Mat planeNormal, double depth);

	// Loads the camera xml data into the XML parser to output K-matrices.
	void loadCameraXMLData(string filename);

	// Load the Raw 4CC images.
	void loadRawImage(string filename, Mat& img0, Mat& img1, Mat& img2,
			Mat& img3);

	// Just displays the DOUBLE matrix after normalization.
	void showMe(Mat img);

	// Returns the NCC image for two images. Does the calculation using SAT
	Mat FastNCC(Mat img1, Mat img2, int patchSize);

	// Takes all 4 images at once to remove redundant calculations for FAST NCC
	// calculation.
	Mat FastNCC(Mat& img1, Mat& img2, Mat& img3, Mat& img4, int patchWidth);

	// Normalize the double matrix and save to disk. If patchWindow = 0, no median
	// filtering is applied.
	void normAndWrite(Mat img, string filename, string stitchedFilame,
			int patchWindow);

	void outputFinalNccImage(vector<Mat>& nccStack, vector<double>&depths,
			int frame_index);

	// Reads the NV21 image and converts it to Mat.
	Mat NV21ToMat(string fileName, int rows, int cols);

	// Find the max element of matrix M, and divide each element of the matrix
	// with it.
	Mat normalizeMat(Mat M);

	// Test using the Summed Area Table implementation against the golden data
	// stored in the /test_data
	void Test_SAT();

	// Using the distortion parameters from the XML, un-distorts and saves the images
	// in the iImages[4] data structure.
	void undistortImages(bool writeToDiskFlag);

	// Prints the matrix type for any OpenCV MAT.
	string type2str(int type);

	// Calculates the core NCC routine using the Summed Area tables for all
	// 4 images together in order to avoid redundant calculations.
	double evaluateWithSAT(int x, int y, int x2, int y2, SATImage& a,
			SATImage& b, SATImage& ab, SATImage& a2, SATImage& b2, double &TH,
			int &n);

	// Helper function to concatenate images.
	Mat createOne(std::vector<cv::Mat> & images, int cols, int min_gap_size);

public:
	// Read the XML data files and setup the engine for a multi-threaded execution.
	static void initWarpEngine(string stereoParamPath, string iRawDataDir);

	// Reads the images.
	void readImages(string frameImageFilename);

	// Runs the actual warping code for a set of frames.
	void runDepthEstimationEngine(int ptr_frame_index);

	// Reads 2 sets of pngs that have 2 4CC camera images each.
	// Split them correctly into 4 separate images.
	void readPngDoubleSetImages(string frameImageFilename1,
			string frameImagefile2);

	// Simpler version of the engine to run only for warping and undistortion test.
	// Called only when testing with checkerboard data.
	void runCheckerboardTestEngine();
};

#endif /* DEPTHESTIMATOR_H_ */
