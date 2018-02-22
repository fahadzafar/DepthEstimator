// This class is responsible for reading the input parameters that are input
// from the command line. Then the data must be parsed, tokenized and checked.
// Once all that is done, set these parameters values to the static data members
// which the program uses after converting them to the correct data type.

#ifndef CMDPARAM_H_
#define CMDPARAM_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class CMDParam {
private:
	// Tokenize based on the delimiter and save it to the input vector
	// data structure.
	static void tokenize(const string& str, vector<string>& tokens,
			string delimiters);

	// Remove the ' ' from the each string in the vector.
	static void removeSpacesFromVector(vector<string>& tokens);

	// Contains the input arguments from the input parameter file.
	static map<string, string> inputArg;

	// Helper function to check if the value exists in the map.
	static bool isPresentInMap(string val, map<string, string>::iterator& it);

	// Utility function to help the printing parameter value and label in the correct format.
	static void printParamsUtil(string a, float value);

	// Checks to see if the variable is within the low and high range. quits if the value is
	// less then low or greater than high. If the tes fails, the program exits.
	static void printCheckError(string label, float val, float low, float high);

public:
	// Reads and sets the input parameters into the static members of OCVInputs.cpp
	static string readFilenameParam(int argc, char* argv[]);

	// Read the parameters from the .txt filename.
	static void readParamsFromFile(string filename);

	// Now that the input arguments have been read and tokenized into the map, set them
	// to the static input arguments to the program.
	static void setTokenizedMapParams();

	// Outputs the read arguments on to the console.
	static void printParams();

	// Declare the input argument token names. These are the values that must be present
	// in the input parameter file.
	static const string TOKEN_NUM_THREAD;
	static const string TOKEN_IMAGE_USED_FOR_DEPTH_CAL;
	static const string TOKEN_IMAGE_WIDTH;
	static const string TOKEN_IMAGE_HEIGHT;
	static const string TOKEN_PLANE_NORMAL;
	static const string TOKEN_PATCH_AREA_NCC;
	static const string TOKEN_DEPTH_START_FROM;
	static const string TOKEN_DEPTH_END_AT;
	static const string TOKEN_DEPTH_INCREMENT;
	static const string TOKEN_START_READ;
	static const string TOKEN_END_READ;
	static const string TOKEN_PATCH_AREA_MEDIAN;
	static const string TOKEN_FIX_FOR_DISTORTION;
	static const string TOKEN_DEPTH_MAP_DIR_GREY;
	static const string TOKEN_DEPTH_MAP_DIR_BIN;
	static const string TOKEN_DEPTH_MAP_DIR_STITCHED;
	static const string TOKEN_PATH_INPUT_RAW_DATA;
	static const string TOKEN_PATH_INPUT_STEREO_PARAM_XML_DATA;
	static const string TOKEN_CHECKERBOARD_TEST;
	static const string TOKEN_NCC_THRESHOLD;
};

#endif /* CMDPARAM_H_ */
