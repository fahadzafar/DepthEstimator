#include <opencv/cv.h>

#include "OCVInputs.h"

using namespace cv;

int In::NUM_THREADS = 8;
int In::IMAGES_USED_FOR_DEPTH_CALCULATION = 4;


// Resolution of each image read from the camera .RAW file.
// These resolution parameters are also used as dimensions for
// output images from the depth estimation engine.
int In::imageWidth = 400;
int In::imageHeight = 400;

// The normal of the depth plane where the image is .
double d[] = { 0, 0, -1 };
Mat In::planeNormal = Mat(3, 1, CV_64F, d);

// NCC Patch area.
float In::patchAreaNCC = 11;

// Depth plane range and the step size used for the plane
// sweep algorithm.
float In::depthStartFrom = 150;
float In::depthEndAt = 550;
float In::depthIncrement = 5;

// These are the .raw frames that must be read from the input directory.
int In::startRead = 3;
int In::endRead = 300;

// Apply the median filter to the final output.
int In::patchAreaMedian = 5;

bool In::fix4ForDistortion = true;

// Only true when checker board test is running.
bool In::checkerboardTest = false;

// The NCC threshold
double In::nccThreshold = 0.01;

// Output directories for images and bin files.
string In::depthMapDirGrey = "depth_maps_grey";
string In::depthMapDirBin = "depth_maps_bin";
string In::depthMapDirStitched = "depth_maps_stitched";

// Input data paths for .raw images and the STereoParameter XML files.
string In::pathInputRAWData = "input_frame_data/face_closeup_7_29_14/";
string In::pathInputStereParamXMLData =
		"/home/local/ANT/fahadz/workspace/OpenCVImageAllignment/input_frame_data/camera_meta_data/StereoParameters_B0F103E134240389_";
