/*
 * StringHelper.h
 *
 *  Created on: Jun 10, 2014
 *      Author: fahadz
 */

#ifndef STRINGHELPER_H_
#define STRINGHELPER_H_

#include <sstream>
#include <string>

using namespace std;

class StringHelper {

	static ostringstream convert;
public:
	static string intToStr(float iData);
};

// Converts int to string using a new object in the
// function making it thread safe.
string threadSafeIntToStr(ostringstream& convert, float iData);

// Converts a string integer to a 3 digit string adding
// the extra 0s in front of it.
string threeDigitString(string name);

#endif /* STRINGHELPER_H_ */
