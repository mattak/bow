#ifndef __HEADER_UTIL
#define __HEADER_UTIL
#include <vector>
#include <string>
#include <map>

using namespace cv;
using namespace std;

vector<string> split(string str, string delim);
void put_as_libsvm (const Mat& mat, ostream& out = cout, const char *label = NULL);

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


class DataIO {
protected:
	bool io_binary;
public:
	HeaderInfo headerinfo;

	DataIO()  { io_binary = true; }
	~DataIO() {;}

	void set_io_binary(bool bin);
	void save(const char* file, const bool bin=true);
	void load(const char* file);

	virtual ostream& write(ostream& os, const bool bin=true) = 0;
	virtual bool read(istream& is, HeaderInfo& info) = 0;
	bool read(istream& is);

	friend ostream& operator <<(ostream &os, DataIO& manipulator) {
		return manipulator.write(os, manipulator.io_binary);
	}
	friend istream& operator >>(istream &is, DataIO& manipulator) {
	  manipulator.read(is);
	  return is;
	}
};

#endif

