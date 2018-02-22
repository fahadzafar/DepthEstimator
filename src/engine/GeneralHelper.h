/*
 * GeneralHelper.h
 *
 *  Created on: Jun 20, 2014
 *      Author: fahadz
 */

#ifndef GENERALHELPER_H_
#define GENERALHELPER_H_

#include <ctime>
#include <map>
#include <string>

#include "Matrices.h"

using namespace std;

// Calculates the difference between two clock timings.
double diffclock(clock_t clock1, clock_t clock2);

// Resets the one directory only.
void resetOneDir(string dir);

#endif /* GENERALHELPER_H_ */
