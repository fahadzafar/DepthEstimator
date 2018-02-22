/*
 * CMDParam.cpp
 *
 *  Created on: Aug 5, 2014
 *      Author: fahadz
 */

#include "CMDParam.h"

#include <fstream>
#include <iostream>
#include <map>
#include <stdlib.h>
#include <string>

#include "OCVInputs.h"

using namespace std;

map<string, string> CMDParam::inputArg;

const string CMDParam::TOKEN_NUM_THREAD = "num_threads";
const string CMDParam::TOKEN_IMAGE_USED_FOR_DEPTH_CAL =
		"image_used_for_depth_cal";
const string CMDParam::TOKEN_IMAGE_WIDTH = "image_width";
const string CMDParam::TOKEN_IMAGE_HEIGHT = "image_height";
const string CMDParam::TOKEN_PLANE_NORMAL = "plane_normal";
const string CMDParam::TOKEN_PATCH_AREA_NCC = "patch_area_ncc";
const string CMDParam::TOKEN_DEPTH_START_FROM = "depth_start_from";
const string CMDParam::TOKEN_DEPTH_END_AT = "depth_end_at";
const string CMDParam::TOKEN_DEPTH_INCREMENT = "depth_increment";
const string CMDParam::TOKEN_START_READ = "start_read";
const string CMDParam::TOKEN_END_READ = "end_read";
const string CMDParam::TOKEN_PATCH_AREA_MEDIAN = "patch_area_median";
const string CMDParam::TOKEN_FIX_FOR_DISTORTION = "fix_for_distortion";
const string CMDParam::TOKEN_DEPTH_MAP_DIR_GREY = "depth_map_dir_grey";
const string CMDParam::TOKEN_DEPTH_MAP_DIR_BIN = "depth_map_dir_bin";
const string CMDParam::TOKEN_DEPTH_MAP_DIR_STITCHED = "depth_map_dir_stitched";
const string CMDParam::TOKEN_PATH_INPUT_RAW_DATA = "path_input_raw_data";
const string CMDParam::TOKEN_PATH_INPUT_STEREO_PARAM_XML_DATA =
		"path_input_stere_param_xml_data";
const string CMDParam::TOKEN_CHECKERBOARD_TEST = "checkerboard_test";
const string CMDParam::TOKEN_NCC_THRESHOLD = "ncc_threshold";

string CMDParam::readFilenameParam(int argc, char* argv[]) {
	string errorMsg = "Error: Usage is ./exe -input parameterFile.txt\n";

	if (argc < 3) {
		cout << errorMsg;
		exit(0);
	} else { // if we got enough parameters...
		char* inputParamFilename = NULL;
		if (string(argv[1]) == string("-input")) {
			inputParamFilename = argv[2];
			return string(inputParamFilename);
		}

		else {
			cout << errorMsg;
			exit(0);
		}
	}
}

void CMDParam::readParamsFromFile(string filename) {
	ifstream myfile(filename.c_str());
	string line;
	if (myfile.is_open()) {
		int count = 0;
		while (getline(myfile, line)) {
			// All parameters in the .txt file have an equal.
			string::size_type position = line.find("=");

			// Read parameters from the string if it does not contain "//" and
			// it should have an "=" sign. Otherwise, ignore the line.
			if (position != string::npos && (line.find("//") == string::npos)) {
				vector<string> tokens;
				tokenize(line, tokens, "=");
				removeSpacesFromVector(tokens);
				inputArg[tokens[0]] = tokens[1];
			}

			count++;
		}
		myfile.close();
	} else {
		cout << " The file cannot be opened." << endl;
	}
}

bool CMDParam::isPresentInMap(string val, map<string, string>::iterator& it) {
	it = inputArg.find(val);
	if (it == inputArg.end()) {
		cout << "Parameter not present: " << val << endl;
		cout << "Check spelling ..." << endl;
		exit(0);
	}
	return true;
}

void CMDParam::printCheckError(string label, float val, float low, float high) {
	if ((val < low) || (val > high)) {
		cout << "********** PARAMETER VALUE ERROR  **********************"
				<< endl;
		cout << "Error in the parameter read from file: " << label << endl;
		cout << "Value of this variable is: " << val << ", should be between "
				<< low << " and " << high << endl;
		cout << "*********************************" << endl;
		exit(0);
	}
}

void CMDParam::setTokenizedMapParams() {
	map<string, string>::iterator it;

	if (isPresentInMap(TOKEN_NUM_THREAD, it)) {
		In::NUM_THREADS = atoi(it->second.c_str());
		printCheckError(TOKEN_NUM_THREAD, In::NUM_THREADS, 1, 16);
	}
	if (isPresentInMap(TOKEN_IMAGE_USED_FOR_DEPTH_CAL, it)) {
		In::IMAGES_USED_FOR_DEPTH_CALCULATION = atoi(it->second.c_str());
		printCheckError(TOKEN_IMAGE_USED_FOR_DEPTH_CAL,
				In::IMAGES_USED_FOR_DEPTH_CALCULATION, 2, 4);
	}
	if (isPresentInMap(TOKEN_IMAGE_WIDTH, it)) {
		In::imageWidth = atoi(it->second.c_str());
		printCheckError(TOKEN_IMAGE_WIDTH, In::imageWidth, 400, 400);

	}
	if (isPresentInMap(TOKEN_IMAGE_HEIGHT, it)) {
		In::imageHeight = atoi(it->second.c_str());
		printCheckError(TOKEN_IMAGE_HEIGHT, In::imageHeight, 400, 400);
	}
	if (isPresentInMap(TOKEN_PLANE_NORMAL, it)) {
		// --- special case since the input is a vector.
		// Remove the '{' and '}'.
		string vecValue = it->second;
		vecValue.erase(vecValue.find('}'), 1);
		vecValue.erase(vecValue.find('{'), 1);

		vector<string> tokens;
		tokenize(vecValue, tokens, ",");
		removeSpacesFromVector(tokens);
		if (tokens.size() != 3) {
			cout << "Parameter error:" << TOKEN_PLANE_NORMAL << endl;
			cout << "Vector not entered correctly." << endl;
			exit(0);
		}

		In::planeNormal.at<double>(0, 0) = atof(tokens[0].c_str());
		In::planeNormal.at<double>(1, 0) = atof(tokens[1].c_str());
		In::planeNormal.at<double>(2, 0) = atof(tokens[2].c_str());
	}
	if (isPresentInMap(TOKEN_PATCH_AREA_NCC, it)) {
		In::patchAreaNCC = atoi(it->second.c_str());
		printCheckError(TOKEN_PATCH_AREA_NCC, In::patchAreaNCC, 1, 21);
	}
	if (isPresentInMap(TOKEN_DEPTH_START_FROM, it)) {
		In::depthStartFrom = atof(it->second.c_str());
	}
	if (isPresentInMap(TOKEN_DEPTH_END_AT, it)) {
		In::depthEndAt = atof(it->second.c_str());
	}
	if (isPresentInMap(TOKEN_DEPTH_INCREMENT, it)) {
		In::depthIncrement = atof(it->second.c_str());
	}
	if (isPresentInMap(TOKEN_START_READ, it)) {
		In::startRead = atoi(it->second.c_str());
	}
	if (isPresentInMap(TOKEN_END_READ, it)) {
		In::endRead = atoi(it->second.c_str());
	}
	if (isPresentInMap(TOKEN_PATCH_AREA_MEDIAN, it)) {
		In::patchAreaMedian = atoi(it->second.c_str());
		printCheckError(TOKEN_PATCH_AREA_MEDIAN, In::patchAreaMedian, 1, 21);
	}
	if (isPresentInMap(TOKEN_FIX_FOR_DISTORTION, it)) {
		// ---- special case since the input is a boolean.
		In::fix4ForDistortion = false;
		if (it->second.c_str() == string("true")) {
			In::fix4ForDistortion = true;
		}
	}
	if (isPresentInMap(TOKEN_CHECKERBOARD_TEST, it)) {
		// ---- special case since the input is a boolean.
		In::checkerboardTest = false;
		if (it->second.c_str() == string("true")) {
			In::checkerboardTest = true;
		}
	}
	if (isPresentInMap(TOKEN_DEPTH_MAP_DIR_GREY, it)) {
		In::depthMapDirGrey = it->second;
	}
	if (isPresentInMap(TOKEN_DEPTH_MAP_DIR_BIN, it)) {
		In::depthMapDirBin = it->second;
	}
	if (isPresentInMap(TOKEN_DEPTH_MAP_DIR_STITCHED, it)) {
		In::depthMapDirStitched = it->second;
	}
	if (isPresentInMap(TOKEN_PATH_INPUT_RAW_DATA, it)) {
		In::pathInputRAWData = it->second;
	}
	if (isPresentInMap(TOKEN_PATH_INPUT_STEREO_PARAM_XML_DATA, it)) {
		In::pathInputStereParamXMLData = it->second;
	}
	if (isPresentInMap(TOKEN_NCC_THRESHOLD, it)) {
		In::nccThreshold = atof(it->second.c_str());
	}
}

void CMDParam::printParamsUtil(string label, float value) {
	cout << label << ": " << value << endl;
}
void CMDParam::printParams() {
	cout << " ----------------------------------------------------------------"
			<< endl;
	cout
			<< " -------------------- INPUT PARAMETERS READ FROM FILE -----------\n"
			<< endl;
	printParamsUtil(TOKEN_NUM_THREAD, In::NUM_THREADS);
	printParamsUtil(TOKEN_IMAGE_USED_FOR_DEPTH_CAL,
			In::IMAGES_USED_FOR_DEPTH_CALCULATION);
	printParamsUtil(TOKEN_IMAGE_WIDTH, In::imageWidth);
	printParamsUtil(TOKEN_IMAGE_HEIGHT, In::imageHeight);

	cout << TOKEN_PLANE_NORMAL << ": " << In::planeNormal << endl;

	printParamsUtil(TOKEN_PATCH_AREA_NCC, In::patchAreaNCC);
	printParamsUtil(TOKEN_DEPTH_START_FROM, In::depthStartFrom);
	printParamsUtil(TOKEN_DEPTH_END_AT, In::depthEndAt);
	printParamsUtil(TOKEN_DEPTH_INCREMENT, In::depthIncrement);
	printParamsUtil(TOKEN_START_READ, In::startRead);
	printParamsUtil(TOKEN_END_READ, In::endRead);
	printParamsUtil(TOKEN_PATCH_AREA_MEDIAN, In::patchAreaMedian);
	printParamsUtil(TOKEN_NCC_THRESHOLD, In::nccThreshold);

	cout << TOKEN_FIX_FOR_DISTORTION << ": ";
	In::fix4ForDistortion ? cout << "true" : cout << "false";
	cout << endl;

	cout << "\n";
	cout << TOKEN_DEPTH_MAP_DIR_GREY << ": " << In::depthMapDirGrey << endl;
	cout << "\n";
	cout << TOKEN_DEPTH_MAP_DIR_BIN << ": " << In::depthMapDirBin << endl;
	cout << "\n";
	cout << TOKEN_DEPTH_MAP_DIR_STITCHED << ": " << In::depthMapDirStitched
			<< endl;
	cout << "\n";
	cout << TOKEN_PATH_INPUT_RAW_DATA << ": " << In::pathInputRAWData << endl;
	cout << "\n";
	cout << TOKEN_PATH_INPUT_STEREO_PARAM_XML_DATA << ": "
			<< In::pathInputStereParamXMLData << endl;

	cout << TOKEN_CHECKERBOARD_TEST << ": ";
	In::checkerboardTest ? cout << "true" : cout << "false";
	cout << endl;

	cout
			<< " \n----------------------------------------------------------------"
			<< endl;
	cout << " ----------------------------------------------------------------"
			<< endl;
	cout
			<< " ----------------------------------------------------------------\n\n\n"
			<< endl;
}

void CMDParam::tokenize(const string& str, vector<string>& tokens,
		string delimiters) {
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);

	// Find first "non-delimiter".
	string::size_type pos = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos) {
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}

void CMDParam::removeSpacesFromVector(vector<string>& tokens) {
	for (unsigned int i = 0; i < tokens.size(); i++) {
		while (tokens[i].find(' ') != string::npos) {
			tokens[i].erase(tokens[i].find(' '), 1);
		}
	}
}
