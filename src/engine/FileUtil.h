// This file reads and writes bin files on the disk.

#include <string>

using namespace std;

namespace FileUtil {
// Writes a binary file with the doubles.
void writeBinFile(string filename, double *ptr, int width, int height);

// Reads a binary file with doubles.
// Size is determined from the binary file.
void readBinFile(string filename, double *ptr, int h, int w);
}

