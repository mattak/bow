#ifndef __HEADER_BOW
#define __HEADER_BOW

#include <cv.h>
#include <cxcore.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define FEATURE_TYPE_SURF "surf"
#define FEATURE_TYPE_SIFT "sift"

using namespace std;
using namespace cv;

class Feature {
protected:
	vector<int> xgrids;
	vector<int> ygrids;
	vector<int> sgrids;
	string loadfile;
public:
	string type;
	Mat descriptor;
	vector<KeyPoint> keypoint;
	
	// constractor	
	Feature();
	~Feature();

	void add_grid(int wstep, int hstep, int scale);

	void extract(const char *type, const char *imgfile);
	
	void show();

	// io
	void save(const char* file,const bool bin=false);
	void load(const char* file,const bool bin=false);
	void write(ofstream& ofs);
	void read(ifstream& ifs);

	void bwrite(ofstream& ofs);
	void bread(ifstream& ifs);
};

#endif

