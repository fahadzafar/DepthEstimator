#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <iostream>

#include "GeneralHelper.h"
#include "Matrices.h"

using namespace std;

double diffclock(clock_t clock1, clock_t clock2) {
	double diffticks = clock1 - clock2;
	double diffms = (diffticks) / (CLOCKS_PER_SEC);
	return diffms;
}

void resetOneDir(string dir) {
	// Check to make sure all the data is not deleted if
	// the input to this function is empty string by accident.
	if (dir == "" || dir == " ")
		return;

	// Remove Old Dir and re-create the folder
	system((string("rm -r ") + dir).c_str());
	system((string("mkdir ") + dir).c_str());
}

