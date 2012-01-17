#include <cv.h>
#include <cxcore.h>
#include <vector>
#include <string>
#include <fstream>
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

void put_as_libsvm (const Mat& mat, ostream& out, const char *label) {
	for (int y=0;y<mat.rows;y++) {
		if (label!=NULL) {
			out << label << " ";
		}
		out << "1:" << mat.at<float>(y,0);
		for (int x=1;x<mat.cols;x++) {
			out << " " << (x+1) << ":" << mat.at<float>(y,x);
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
		it++;
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

// DataIO
//--------
void DataIO::set_io_binary (bool bin) {
	io_binary = bin;
}

void DataIO::save (const char* file, const bool bin) {
	ofstream ofs;
	ofs.open(file, ios::out|ios::binary);
	write(ofs,bin);
	ofs.close();
}

void DataIO::load (const char* file) {
	ifstream ifs;
	ifs.open(file, ios::in|ios::binary);
	read(ifs);
	ifs.close();
}

bool DataIO::read (istream& is) {
	headerinfo.init();
	is >> headerinfo;
	if (headerinfo.data.empty()) {
		return false;
	}
	return read(is, headerinfo);
}


