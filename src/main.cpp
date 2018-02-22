// This file is the main driver of the depth estimating utility. The main function
// resides in this file. The main purpose of this file is to divide the work
// equally amongst all the execution threads (In::NUM_THREADS). The camera
// parameters are read into static variables, hence they only need to be read once.
// After that, the compute is called per frame.

#include <iostream>
#include <pthread.h>
#include <sstream>
#include <string>

#include "CMDParam.h"
#include "DepthEstimator.h"
#include "GeneralHelper.h"
#include "StringHelper.h"

using namespace std;

// The function that runs on each CPU thread.
void *threadedPlaneSweep(void *t);

// Test with known checker board data.
void runCheckerBoardTest();

// Structure used to pass data to the thread.
struct threadData {
	int x;
};

int main(int argc, char* argv[]) {
	// Read the input parameter filename and the data from that param file.

	string paramFile = CMDParam::readFilenameParam(argc, argv);
	if (paramFile == "") {
		cout << "****************" << endl;
		cout << "No input parameter file entered\n";
		cout << "****************" << endl;
		exit(0);
	} else {
		CMDParam::readParamsFromFile(paramFile);
		CMDParam::setTokenizedMapParams();
		CMDParam::printParams();
	}

	// Make sure the output depth map directories are reset to empty.
	resetOneDir(In::pathInputRAWData + In::depthMapDirGrey);
	resetOneDir(In::pathInputRAWData + In::depthMapDirBin);
	resetOneDir(In::pathInputRAWData + In::depthMapDirStitched);

	// Initialize the static variables and read the camera parameter
	// data from the StereoParameter XML files.
	DepthEstimator::initWarpEngine(In::pathInputStereParamXMLData,
			In::pathInputRAWData);

	if (In::checkerboardTest) {
		// Run the special checker board test.
		runCheckerBoardTest();
		exit(0);
	}

	//--------------------------- Threading the execution.
	// Initialize the thread parameters.
	int rc;
	pthread_t threads[In::NUM_THREADS];
	pthread_attr_t attr;
	void *status;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	threadData threadData[In::NUM_THREADS];

	// Call all the threads on the frame sequence.
	for (int k = 0; k < In::NUM_THREADS; k++) {
		threadData[k].x = k;

		rc = pthread_create(&threads[k], NULL, threadedPlaneSweep,
				(void*) &threadData[k]);

		if (rc) {
			cout << "Error:unable to create thread," << rc << endl;
			exit(-1);
		}
	}

	// free attribute and wait for the other threads
	pthread_attr_destroy(&attr);
	for (int i = 0; i < In::NUM_THREADS; i++) {
		rc = pthread_join(threads[i], &status);
		if (rc) {
			cout << "Error:unable to join," << rc << endl;
			exit(-1);
		}
	}
	pthread_exit(NULL);
	cout << "-----------> Threaded Plane Sweep completed" << endl;
	cout << "-----------COMPLETE !!!------------------------------" << endl;
	return 0;
}

void runCheckerBoardTest() {
	cout << "CHECKER BOARD TEST START" << endl;
	DepthEstimator de;
	de.readPngDoubleSetImages(
			In::pathInputRAWData + string("frame03_2_1,0_12_001.png"),
			In::pathInputRAWData + string("frame03_2_3,2_34_001.png"));
	de.runCheckerboardTestEngine();
	cout << "CHECKER BOARD TEST END" << endl;
}

// This function just calls the planeSween and NCC calculation on all the
// data set based on In::NUM_THREAD. The camera XML data is loaded as static
// so it can be shared amongst threads.
void *threadedPlaneSweep(void *t) {
	// Read the thread specific data.
	threadData * ptr = (threadData*) t;
	int id = ptr->x;
	ostringstream ss;
	int count = 0;
	DepthEstimator de;

	for (int i = id + count * In::NUM_THREADS + In::startRead; i < In::endRead;
			i += In::NUM_THREADS) {
		String filename = In::pathInputRAWData + string("frame_2_0,1,2,3_4321_")
				+ threeDigitString(threadSafeIntToStr(ss, i)) + string(".raw");
		string rawImageFilename = filename;
		de.readImages(rawImageFilename);
		de.runDepthEstimationEngine(i);
		count++;
	}
	pthread_exit(NULL);
}
