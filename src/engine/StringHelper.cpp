#include <string>
#include <iostream>
#include <sstream>

#include "StringHelper.h"
using namespace std;

ostringstream StringHelper::convert;

string StringHelper::intToStr(float iData) {
	convert.str("");
	convert.clear();
	convert << iData;
	return convert.str();
}

string threadSafeIntToStr(ostringstream& convert, float iData) {
	convert.str("");
	convert.clear();
	convert << iData;
	return convert.str();
}

string threeDigitString(string name) {
	if (name.size() == 1)
		return "00" + name;
	else if (name.size() == 2)
		return "0" + name;
	else
		return name;
}
