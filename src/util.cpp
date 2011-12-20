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


// HeaderInfo
//-----------------------
 
void HeaderInfo::init () {
  data.clear();
}

ostream& HeaderInfo::write (ostream& os) {
  map< string, vector<string> >::iterator it = data.begin();
  os << "^" << endl;
  while (it!=data.end()) {
    string tag = (*it).first;
    vector<string> elms = (*it).second;
    cout << tag;
    for(int i=0;i<elms.size();i++) {
      cout << " " << elms.at(i);
    }
    cout << endl;
  }
  os << "$" << endl;
  return os;
}

bool HeaderInfo::read (istream& is) {
  string line;
  bool isheader = false;
  while (is && getline(is,line)) {
    if (line=="^") {
      isheader = true;
      break;
    }
  }
  if (!isheader) {
    return false;
  }
  while (is && getline(is,line)) {
    if (line=="$") { isheader=false; break; }
    vector<string> elms = split(line, " ");
    string tag = elms.front();
    elms.erase(elms.begin());
    data.insert( map< string, vector<string> >::value_type(tag, elms) );
  }
  return !isheader;
}


