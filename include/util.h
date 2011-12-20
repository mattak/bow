#ifndef __HEADER_UTIL
#define __HEADER_UTIL
#include <vector>
#include <string>
#include <map>

using namespace cv;
using namespace std;

vector<string> split(string str, string delim);

void put_as_libsvm (int label, const Mat& mat, ostream& out = cout);

class HeaderInfo {
public:
  map< string,vector<string> > data;
  
  void init();
  
  ostream& write(ostream& os);
  bool read(istream& is);
  
  friend ostream& operator <<(ostream &os, HeaderInfo& manipulator) {
    return manipulator.write(os);
  }
  friend istream& operator >>(istream &is, HeaderInfo& manipulator) {
    manipulator.read(is);
    return is;
  }
};

#endif

