#include <fstream>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#include "FileUtil.h"
#include "GeneralHelper.h"
#include "StringHelper.h"
#include "DepthEstimator.h"
#include "OCVInputs.h"
#include "SATImage.h"

using namespace cv;
using namespace std;
using namespace FileUtil;

// Instantiate the static class members.
StereoParamXML DepthEstimator::K_0_1_file;
StereoParamXML DepthEstimator::K_0_2_file;
StereoParamXML DepthEstimator::K_2_3_file;
StereoParamXML DepthEstimator::K_3_1_file;

Mat DepthEstimator::K[4];
Mat DepthEstimator::trans[4];
Mat DepthEstimator::rot[4];
Mat DepthEstimator::distCoeffs[4];
string DepthEstimator::rawDataDir = "";

void DepthEstimator::initWarpEngine(string stereoParamPath,
		string iRawDataDir) {
	rawDataDir = iRawDataDir;

	// Read the camera input files.
	K_0_1_file.loadXML(stereoParamPath + "0_1.xml");
	K_0_2_file.loadXML(stereoParamPath + "0_2.xml");
	K_2_3_file.loadXML(stereoParamPath + "2_3.xml");
	K_3_1_file.loadXML(stereoParamPath + "3_1.xml");

	K[0] = K_0_1_file.cameraMatrixLeft.clone();
	K[1] = K_0_1_file.cameraMatrixRight.clone();
	K[2] = K_2_3_file.cameraMatrixLeft.clone();
	K[3] = K_3_1_file.cameraMatrixLeft.clone();

	trans[0] = (Mat_<double>(3, 1) << 0, 0, 0);
	rot[0] = (Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);

	trans[1] = K_0_1_file.relativeTranslationLeftToRight * 1000;
	rot[1] = K_0_1_file.relativeRotationLeftToRight;

	trans[2] = K_0_2_file.relativeTranslationLeftToRight * 1000;
	rot[2] = K_0_2_file.relativeRotationLeftToRight;

	trans[3] = (K_2_3_file.relativeRotationLeftToRight * (trans[2] / 1000)
			+ K_2_3_file.relativeTranslationLeftToRight) * 1000;
	rot[3] = K_2_3_file.relativeRotationLeftToRight * rot[2];

	distCoeffs[0] = K_0_1_file.distCoeffsLeft;
	distCoeffs[1] = K_0_1_file.distCoeffsRight;
	distCoeffs[2] = K_2_3_file.distCoeffsLeft;
	distCoeffs[3] = K_3_1_file.distCoeffsLeft;

}

void DepthEstimator::readImages(string frameImageFilename) {
	string rawImageFilename = frameImageFilename;

	// Read the images and split them.
	loadRawImage(rawImageFilename, iImages[0], iImages[1], iImages[2],
			iImages[3]);

	// Undistort the images using the distortion parameters.
	if (In::fix4ForDistortion) {
		undistortImages(false);
	}
}

void DepthEstimator::undistortImages(bool witeToDisk) {
	// Undistort the images using the distortion parameters, fix the image.
	Mat fixedImage;
	cout << "\nWriting the UNDISTORTED data images as: undistorted_x.png"
			<< endl;
	for (int i = 0; i < 4; i++) {
		string name = "undistorted_" + StringHelper::intToStr(i) + ".png";

		undistort(iImages[i], fixedImage, K[i], distCoeffs[i]);
		iImages[i] = fixedImage.clone();
		if (witeToDisk) {
			imwrite(name, iImages[i]);
		}
	}
}

void DepthEstimator::readPngDoubleSetImages(string frameImageFilename1,
		string frameImageFilename2) {

	// Read first file.
	Mat image0_1 = imread(frameImageFilename1);
	Mat image2_3 = imread(frameImageFilename2);

	image0_1.convertTo(image0_1, CV_64FC1);
	image2_3.convertTo(image2_3, CV_64FC1);

	iImages[1] = image0_1(Rect(0, 0, 400, 400)).clone();
	iImages[0] = image0_1(Rect(400, 0, 400, 400)).clone();
	iImages[3] = image2_3(Rect(0, 0, 400, 400)).clone();
	iImages[2] = image2_3(Rect(400, 0, 400, 400)).clone();

	// Write the input images.
	cout << "Writing the input data images as: input_x.png" << endl;
	for (int i = 0; i < 4; i++) {
		string name = "input_" + StringHelper::intToStr(i) + ".png";
		imwrite(name, iImages[i]);
	}

	// Undistort the image.
	if (In::fix4ForDistortion) {
		undistortImages(true);
	}
}

void DepthEstimator::normAndWrite(Mat img, string filename,
		string stitchedFilname, int patchWindow) {
	double minVal;
	double maxVal;
	Point minLoc;
	Point maxLoc;

	minMaxLoc(img, &minVal, &maxVal, &minLoc, &maxLoc);
	Mat norm = img / maxVal;

	// Used to make a color depth map.
	Mat downCast = norm * 255;
	if (patchWindow > 0) {
		Mat medianOutput;

		// Median
		downCast.convertTo(downCast, CV_8U);
		medianBlur(downCast, medianOutput, patchWindow);
		downCast = medianOutput;
	}

	imwrite(filename.c_str(), downCast);

	if (stitchedFilname != string("0")) {
		// Output the stitched image.
		vector<Mat> imgArr;
		imgArr.push_back(imageAllCameras(Rect(400, 0, 400, 400)));
		imgArr.push_back(downCast.clone());

		Mat stitchedImage = createOne(imgArr, 2, 0);
		imwrite(stitchedFilname.c_str(), stitchedImage);
	}
}

cv::Mat DepthEstimator::createOne(std::vector<cv::Mat> & images, int cols,
		int min_gap_size) {
	// let's first find out the maximum dimensions
	int max_width = 0;
	int max_height = 0;
	for (unsigned int i = 0; i < images.size(); i++) {
		// check if type is correct
		// you could actually remove that check and convert the image
		// in question to a specific type
		if (i > 0 && images[i].type() != images[i - 1].type()) {
			std::cerr << "WARNING:createOne failed, different types of images";
			return cv::Mat();
		}
		max_height = std::max(max_height, images[i].rows);
		max_width = std::max(max_width, images[i].cols);
	}
	// number of images in y direction
	int rows = std::ceil(images.size() / cols);

	// create our result-matrix
	cv::Mat result = cv::Mat::zeros(
			rows * max_height + (rows - 1) * min_gap_size,
			cols * max_width + (cols - 1) * min_gap_size, images[0].type());
	size_t i = 0;
	int current_height = 0;
	int current_width = 0;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			if (i >= images.size()) // shouldn't happen, but let's be safe
				return result;
			// get the ROI in our result-image
			cv::Mat to(result,
					cv::Range(current_height, current_height + images[i].rows),
					cv::Range(current_width, current_width + images[i].cols));
			// copy the current image to the ROI
			images[i++].copyTo(to);
			current_width += max_width + min_gap_size;
		}
		// next line - reset width and update height
		current_width = 0;
		current_height += max_height + min_gap_size;
	}
	return result;
}

void DepthEstimator::showMe(Mat img) {
	double minVal;
	double maxVal;
	Point minLoc;
	Point maxLoc;

	minMaxLoc(img, &minVal, &maxVal, &minLoc, &maxLoc);
	Mat norm = img / maxVal;
	imshow("Norm-NCC", norm);
}

void DepthEstimator::runCheckerboardTestEngine() {
	double depth = In::depthStartFrom;
	Size size(iImages[0].cols, iImages[0].rows);
	Mat imgOut[4];

	// Calculate the Homography and warped images.
	cout
			<< "\nWriting the abs(UNDISTORTED_WARPED[0] - UNDISTORTED_WARPED[i] ) data images as: undistort_minus_warped_x.png"
			<< endl;

	// Convert cam 0 image to single channel CV_8UC to place in
	// the green channel for warped output comparison.
	Mat imageCamZero = iImages[0].clone();
	imageCamZero.convertTo(imageCamZero, CV_8UC3);
	cvtColor(imageCamZero, imageCamZero, CV_RGB2GRAY);

	for (int i = 0; i < 4; i++) {
		Mat M = getHomorgrapy(rot[i], trans[i], In::planeNormal, depth);
		Mat F1 = K[0] * M * K[i].inv();
		warpPerspective(iImages[i], imgOut[i], F1, size, CV_INTER_CUBIC,
				BORDER_CONSTANT, 0);

		// Create the output warped image with cam-0 as red channel
		// and the warped_i image in green cahnnel.
		//create a 3 channeled matrix
		imgOut[i].convertTo(imgOut[i], CV_8UC3);

		// cout << "imgOut:" << type2str(imgOut[i].type()) << endl;
		// cout << "iImage:" << type2str(iImages[0].type()) << endl;

		vector<Mat> each_channel;
		split(imgOut[i], each_channel);

		// All 3 channels contain the same data since its a 3 channel gray image.
		// Leave the data in the Red channel, set the blue channel to 0 and
		// set the green channel to cam-0 image.
		each_channel[0] = Mat::zeros(iImages[0].cols, iImages[0].rows,
		CV_8UC1);
		each_channel[1] = imageCamZero;
		merge(each_channel, imgOut[i]);

		imwrite("warped_to_0" + StringHelper::intToStr(i) + ".png", imgOut[i]);
		string name = "undistort_minus_warped_" + StringHelper::intToStr(i)
				+ ".png";
		imwrite(name, abs(imgOut[0] - imgOut[i]));
	}

}
void DepthEstimator::runDepthEstimationEngine(int frame_index) {
	cout << "\n ------------------NCC START for " << frame_index << endl;
	vector<Mat> nccStack;
	vector<double> normDepths;

	// Now align the images.
	Size size(iImages[0].cols, iImages[0].rows);
	double depth = 0;

	int totalNCCs = (In::depthEndAt - In::depthStartFrom) / In::depthIncrement
			+ 1;
	cout << "Total plane sweeps:" << totalNCCs << ", Start:"
			<< In::depthStartFrom << ", End:" << In::depthEndAt
			<< ", depthStep:" << In::depthIncrement << endl;

	// Start timer clock.
	clock_t begin = clock();
	Mat globalNCC;

	double warpingTimeSUM = 0;
	double nccTimeSUM = 0;

	// Instead of doing the up-casting in FastNCC function, do it here
	// for 4 image NCC. For the FastNCC with 2 images, it is done
	// inside the function locally.
	if (In::IMAGES_USED_FOR_DEPTH_CALCULATION == 4) {
		for (int i = 0; i < 4; i++)
			iImages[i].convertTo(iImages[i], CV_64FC1);
	}

	for (float depthsFrom = In::depthStartFrom, index = 0;
			depthsFrom <= In::depthEndAt;
			depthsFrom += In::depthIncrement, index++) {
		depth = depthsFrom;
		Mat imgOut[4];

		clock_t warpingBegin = clock();

		// Calculate the Homography and warped images.
		for (int i = 0; i < 4; i++) {
			Mat M = getHomorgrapy(rot[i], trans[i], In::planeNormal, depth);
			Mat F1 = K[0] * M * K[i].inv();
			warpPerspective(iImages[i], imgOut[i], F1, size, CV_INTER_CUBIC,
					BORDER_CONSTANT, 0);
		}

		// cout << type2str(imgOut[0].type()) << endl;

		clock_t warpingEnd = clock();
		warpingTimeSUM += (diffclock(warpingEnd, warpingBegin) / 10.0);

		// Begin the NCC calculation and its timing.
		clock_t nccBegin = clock();
		if (In::IMAGES_USED_FOR_DEPTH_CALCULATION == 2) {
			Mat NCC01 = FastNCC(imgOut[0], imgOut[1], In::patchAreaNCC);
			globalNCC = NCC01;
		}

		if (In::IMAGES_USED_FOR_DEPTH_CALCULATION == 3) {
			Mat NCC01 = FastNCC(imgOut[0], imgOut[1], In::patchAreaNCC);
			Mat NCC02 = FastNCC(imgOut[0], imgOut[2], In::patchAreaNCC);
			globalNCC = (NCC01 + NCC02) / 2;
		}

		else if (In::IMAGES_USED_FOR_DEPTH_CALCULATION == 4) {
			// Use the faster NCC calculation for 4 images.
			// Also convert the iImage[i] to CV_64F outside the depth loop.
			globalNCC = FastNCC(imgOut[0], imgOut[1], imgOut[2], imgOut[3],
					In::patchAreaNCC);

			// Alternate slower calculation.
			// Mat NCC01 = FastNCC(imgOut[0], imgOut[1], In::patchAreaNCC);
			// Mat NCC02 = FastNCC(imgOut[0], imgOut[2], In::patchAreaNCC);
			// Mat NCC03 = FastNCC(imgOut[0], imgOut[3], In::patchAreaNCC);
			// globalNCC = (NCC01 + NCC02 + NCC03) / 3;
		}

		nccStack.push_back(globalNCC.clone());
		clock_t nccEnd = clock();

		nccTimeSUM += diffclock(nccEnd, nccBegin) / 10.0;

		// Normalize the depth range from 0-1 and store in the depth images.
		normDepths.push_back(
				(depth - In::depthStartFrom)
						/ (In::depthEndAt - In::depthStartFrom));
	} // End depth loop.

	clock_t diskBegin = clock();
	outputFinalNccImage(nccStack, normDepths, frame_index);
	clock_t diskEnd = clock();

	cout << "Execution time (for NCC): " << nccTimeSUM << " seconds." << endl;
	cout << "Execution time (for WARPING): " << warpingTimeSUM << " seconds."
			<< endl;
	cout << "Execution time (for SAVE-DISK): "
			<< diffclock(diskEnd, diskBegin) / 10.0 << " seconds." << endl;

	// End timer here
	clock_t end = clock();
	cout << "Execution time (COMPLETE): " << diffclock(end, begin) / 10.0
			<< " seconds." << endl;
	cout << "NCC End" << endl;
}

void DepthEstimator::outputFinalNccImage(vector<Mat>& nccStack,
		vector<double>& depths, int frame_index) {

	cout << "Calculating Max NCC image" << endl;
	Mat imgDepthOut = Mat::zeros(400, 400, CV_64FC1);
	Mat imgNCCData = Mat::zeros(400, 400, CV_64FC1);

	imgDepthOut = depths[0];

	// NCC is -1 to 1 so initialize to a smaller value.
	imgNCCData = -10;

	uint8_t maxGray = 255;
	for (unsigned int k = 0; k < nccStack.size(); k++) {
		for (int j = 0; j < In::imageHeight; j++) {
			for (int i = 0; i < In::imageWidth; i++) {
				if (nccStack[k].at<double>(j, i)
						> imgNCCData.at<double>(j, i)) {
					imgNCCData.at<double>(j, i) = nccStack[k].at<double>(j, i);
					imgDepthOut.at<double>(j, i) = depths[k] * maxGray;

				}
			}
		}
	}

	ostringstream ss;
	string depthmap_filename = threeDigitString(
			threadSafeIntToStr(ss, frame_index));
	string nccraw_filename = threadSafeIntToStr(ss, frame_index);

	writeBinFile(
			rawDataDir + In::depthMapDirBin + "/" + depthmap_filename + ".bin",
			(double*) imgDepthOut.data, 400, 400);

	string stitchedFilename = "0";
	if (In::depthMapDirStitched != string("0")) {
		stitchedFilename = rawDataDir + In::depthMapDirStitched + "/depthMap_"
				+ depthmap_filename + ".png";
	}
	normAndWrite(imgDepthOut,
			rawDataDir + In::depthMapDirGrey + "/depthMap_" + depthmap_filename
					+ ".png", stitchedFilename, In::patchAreaMedian);
}

string DepthEstimator::type2str(int type) {
	string r;

	uchar depth = type & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (type >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:
		r = "8U";
		break;
	case CV_8S:
		r = "8S";
		break;
	case CV_16U:
		r = "16U";
		break;
	case CV_16S:
		r = "16S";
		break;
	case CV_32S:
		r = "32S";
		break;
	case CV_32F:
		r = "32F";
		break;
	case CV_64F:
		r = "64F";
		break;
	default:
		r = "User";
		break;
	}

	r += "C";
	r += (chans + '0');

	return r;
}

void DepthEstimator::Test_SAT() {
	cout << "Testing integral image generation" << endl;
	Mat img1 = imread("../test_data/sat_ncc_data/testa.png",
			CV_LOAD_IMAGE_GRAYSCALE);
	Mat img2 = imread("../test_data/sat_ncc_data//testb.png",
			CV_LOAD_IMAGE_GRAYSCALE);

	img1.convertTo(img1, CV_64FC1);
	img2.convertTo(img2, CV_64FC1);

	img1 = img1.t();
	img2 = img2.t();

	Mat buff = img2.mul(img1);
	SATImage ocv_ab(buff);

// Read the golden file data.
	double *goldenDataa_by_b = (double*) calloc(400 * 400, sizeof(double));
	double *goldenData_ncc_b = (double*) calloc(400 * 400, sizeof(double));
	readBinFile("../test_data/sat_ncc_data/a_mul_b_sat.bin", goldenDataa_by_b,
			400, 400);
	readBinFile("../test_data/sat_ncc_data/a_b_ncc_patch_7.bin",
			goldenData_ncc_b, 400, 400);

	Mat calculatedNCC = FastNCC(img1, img2, 7);

	int w = 400;
	int patchW = 7;
	int k = floor(patchW / 2.);

	for (int i = k; i < w - k; i++) {
		for (int j = k; j < w - k; j++) {
			int x = i - k;
			int y = j - k;
			int x2 = i + k;
			int y2 = j + k;

			double sum = ocv_ab.EvaluateSAT(x, y, x2, y2);

			if (goldenDataa_by_b[j * w + i] != sum) {
				cout << "FAIL, fromFile:" << goldenDataa_by_b[j * w + i]
						<< ", calculated:" << sum << endl;
				exit(0);
			}
		}
	}

// Check the NCC values between A and B
	for (int i = k; i < w - k; i++) {
		for (int j = k; j < w - k; j++) {
			if (goldenData_ncc_b[j * w + i] != calculatedNCC.at<double>(i, j)) {
				cout << "FAIL, fromFile:" << goldenData_ncc_b[j * w + i]
						<< ", calculated:" << calculatedNCC.at<double>(i, j)
						<< endl;
				exit(0);
			}
		}
	}
	delete goldenData_ncc_b;
	delete goldenDataa_by_b;
	cout << "integral image generation : PASS ................" << endl;
}

Mat DepthEstimator::FastNCC(Mat img1, Mat img2, int patchWidth) {
// Up cast the matrices so all computation is done using doubles.
	img1.convertTo(img1, CV_64F);
	img2.convertTo(img2, CV_64F);

// The reading and writing for Mat is transposed.
// To compensate, fix this.
	img1 = img1.t();
	img2 = img2.t();
	SATImage a(img1);
	SATImage b(img2);

	Mat ncc = Mat::zeros(In::imageWidth, In::imageHeight, CV_64F);

	Mat buff = img1.mul(img1);
	SATImage a2(buff);

	buff = img2.mul(img2);
	SATImage b2(buff);

	buff = img2.mul(img1);
	SATImage ab(buff);

	int k = static_cast<int>(floor(patchWidth / 2.0));
	int n = patchWidth * patchWidth;
	float TH = 0.01;

	for (int i = k; i < In::imageHeight - k; i++) {
		for (int j = k; j < In::imageWidth - k; j++) {

			int x = i - k;
			int y = j - k;
			int x2 = i + k;
			int y2 = j + k;

			double t1 = ab.EvaluateSAT(x, y, x2, y2);
			double t2 = a.EvaluateSAT(x, y, x2, y2);
			double t3 = b.EvaluateSAT(x, y, x2, y2);
			double t4 = a2.EvaluateSAT(x, y, x2, y2);
			double t5 = b2.EvaluateSAT(x, y, x2, y2);

			double num = (n * t1 - t2 * t3);

			// calculates numerator
			double d1 = (n * t4 - t2 * t2);		//calculates var of a over patch
			double d2 = (n * t5 - t3 * t3);

			if (d1 < TH || d2 < TH)
				continue;

			float finalVal = num / sqrt(d1 * d2);
			ncc.at<double>(j, i) = finalVal;
		}
	}

	return ncc;
}

Mat DepthEstimator::FastNCC(Mat& img1, Mat& img2, Mat& img3, Mat& img4,
		int patchWidth) {
// The reading and writing for Mat is transposed.
// To compensate, fix this.
	img1 = img1.t();
	img2 = img2.t();
	img3 = img3.t();
	img4 = img4.t();

	SATImage a(img1);
	SATImage b(img2);
	SATImage c(img3);
	SATImage d(img4);

	Mat ncc = Mat::zeros(In::imageWidth, In::imageHeight, CV_64FC1);

	Mat buff = img1.mul(img1);
	SATImage a2(buff);

	Mat buff2 = img2.mul(img2);
	SATImage b2(buff2);

	Mat buff3 = img3.mul(img3);
	SATImage c2(buff3);

	Mat buff4 = img4.mul(img4);
	SATImage d2(buff4);

	Mat buff5 = img2.mul(img1);
	SATImage ab(buff5);

	Mat buff6 = img3.mul(img1);
	SATImage ac(buff6);

	Mat buff7 = img4.mul(img1);
	SATImage ad(buff7);

	int k = static_cast<int>(floor(patchWidth / 2.0));
	int n = patchWidth * patchWidth;
	double TH = In::nccThreshold;

	for (int i = k; i < In::imageHeight - k; i++) {
		for (int j = k; j < In::imageWidth - k; j++) {

			int x = i - k;
			int y = j - k;
			int x2 = i + k;
			int y2 = j + k;

			double ncc01 = evaluateWithSAT(x, y, x2, y2, a, b, ab, a2, b2, TH,
					n);
			double ncc02 = evaluateWithSAT(x, y, x2, y2, a, c, ac, a2, c2, TH,
					n);
			double ncc03 = evaluateWithSAT(x, y, x2, y2, a, d, ad, a2, d2, TH,
					n);
			ncc.at<double>(j, i) = (ncc01 + ncc02 + ncc03) / 3;
		}
	}
	return ncc;
}

double DepthEstimator::evaluateWithSAT(int x, int y, int x2, int y2,
		SATImage& a, SATImage& b, SATImage& ab, SATImage& a2, SATImage& b2,
		double &TH, int &n) {
	double t1 = ab.EvaluateSAT(x, y, x2, y2);
	double t2 = a.EvaluateSAT(x, y, x2, y2);
	double t3 = b.EvaluateSAT(x, y, x2, y2);
	double t4 = a2.EvaluateSAT(x, y, x2, y2);
	double t5 = b2.EvaluateSAT(x, y, x2, y2);

	double num = (n * t1 - t2 * t3);

// calculates numerator
	double d1 = (n * t4 - t2 * t2);		//calculates var of a over patch
	double d2 = (n * t5 - t3 * t3);

	if (d1 < TH || d2 < TH)
		return 0;

	float finalVal = num / sqrt(d1 * d2);
	return finalVal;
}

void DepthEstimator::loadRawImage(string filename, Mat& img0, Mat& img1,
		Mat& img2, Mat& img3) {
// Read the raw file. Size is fixed to 400 * 1600 * 1.
	cout << "\n reading file:" << filename << "\n" << endl;
	FILE * f = fopen(filename.c_str(), "rb");
	if (!f) {
		printf("bad path : %s\n", filename.c_str());
		exit(0);
	}

// Since there are 4 images in the RAW image.
	int totalPixels = In::imageHeight * (In::imageWidth * 4);
	char *pixels = (char*) calloc(totalPixels, sizeof(char));
	fread(pixels, totalPixels, 1, f);
	fclose(f);

	imageAllCameras =
			Mat(In::imageHeight, In::imageWidth * 4, CV_8U, pixels).clone();

	img2 = imageAllCameras(Rect(0, 0, 400, 400)).clone();
	img1 = imageAllCameras(Rect(400, 0, 400, 400)).clone();
	img0 = imageAllCameras(Rect(800, 0, 400, 400)).clone();
	img3 = imageAllCameras(Rect(1200, 0, 400, 400)).clone();

}

Mat DepthEstimator::normalizeMat(Mat M) {
	double Matmax = abs(M.at<double>(0, 0));
	for (int i = 0; i < M.rows; i++) {
		for (int j = 0; j < M.cols; j++) {
			Matmax = max(abs(M.at<double>(j, i)), Matmax);
		}
	}

// Normalize using the max element.
	for (int i = 0; i < M.rows; i++) {
		for (int j = 0; j < M.cols; j++) {
			M.at<double>(j, i) = M.at<double>(j, i) / Matmax;
		}
	}
	return M;
}

Mat DepthEstimator::getHomorgrapy(Mat rot, Mat trans, Mat planeNormal,
		double depth) {
	bool debug = false;
	Mat norm_by_d = planeNormal / depth;

	Mat subtract = trans * norm_by_d.t();

	if (debug) {
		cout << "==================" << endl;
		cout << "Subtract:" << subtract << endl;
		cout << "R:" << rot << endl;
		cout << "t:" << trans << endl;
		cout << "n:" << planeNormal << endl;
		cout << "d:" << depth << endl;
		cout << "PlaneNormal: " << planeNormal << endl;
		cout << "==================" << endl;
	}
	Mat M(3, 3, CV_64F);
	M = rot - subtract;

// Find the max element.
	Mat H2 = normalizeMat(M);

	M = normalizeMat(H2.inv());
	return M;
}

