/*
 * FileUtil.cpp
 *
 *  Created on: Jul 15, 2014
 *      Author: fahadz
 */

#include <FileUtil.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;
using namespace FileUtil;

void FileUtil::writeBinFile(string filename, double *ptr, int width,
		int height) {
	// Create a file with the given filename.
	ofstream myFile(filename.c_str(), ios::out | ios::binary);
	double info[3];
	info[0] = height;
	info[1] = width;
	info[2] = 1;

	// Write the date and close.
	myFile.write((char*) info, 3 * 8);
	myFile.write((char*) ptr, width * height * 8);
	myFile.close();
}

void FileUtil::readBinFile(string filename, double *ptr, int height, int width) {
	ifstream myFile(filename.c_str(), ios::in | ios::binary);
	// Read the meta information.
	double info[3];
	myFile.read((char*) info, 8 * 3);

	if (height != info[0] || width !=info[1] || 1!=info[2] ){
		cout << "Readbinfile, file does not have the correct channel/row/cols:" << endl;
		exit(0);
	}

	myFile.read((char*) ptr, height * width * 8);
	myFile.close();
}


