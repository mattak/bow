#ifndef __HEADER_UTIL
#define __HEADER_UTIL
#include <vector>
#include <string>

using namespace cv;
using namespace std;

vector<string> split(string str, string delim);

void put_as_libsvm (int label, const Mat& mat);

#endif

