#include <cv.h>
#include <cxcore.h>
#include <vector>
#include <string>
#include "util.h"

using namespace cv;
using namespace std;

// private
vector<string> split(string str, string delim) {
  vector<string> result;
  int cutAt;
  while ((cutAt = str.find_first_of(delim))!=str.npos) {
    if (cutAt>0) {
      result.push_back(str.substr(0,cutAt));
    }
    str = str.substr(cutAt+1);
  }
  if (str.length()>0) {
    result.push_back(str);
  }
  return result;
}

void put_as_libsvm (int label, const Mat& mat, ostream& out) {
	for (int y=0;y<mat.rows;y++) {
		out << label;
		for (int x=0;x<mat.cols;x++) {
			out << " " << x << ":" << mat.at<float>(y,x);
		}
		out << endl;
	}
}

