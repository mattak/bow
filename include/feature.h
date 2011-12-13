#ifndef __HEADER_FEATURE
#define __HEADER_FEATURE

#include <cv.h>
#include <cxcore.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#define FEATURE_TYPE_SURF "surf"
#define FEATURE_TYPE_SIFT "sift"
#define FEATURE_TYPE_ORB  "orb"

using namespace std;
using namespace cv;

class Feature {
protected:
	vector<int> xgrids;
	vector<int> ygrids;
	vector<int> sgrids;
	string loadfile;
public:
	enum DataType {FLOAT, UCHAR};

	string type;
	Mat descriptor;
	vector<KeyPoint> keypoint;
	
	// constractor	
	Feature();
	~Feature();

	void add_grid(int wstep, int hstep, int scale);

	void extract(const char *type, const char *imgfile);
  void extract(const char *type, Mat& grayimg);
	
	void show();

	// io
	void save(const char* file,const bool bin=true);
	void load(const char* file);
	void write(ofstream& ofs, const bool bin=true);
	bool read(ifstream& ifs);

	// util
	Mat floated_descriptor();
	DataType data_type();


  friend ostream& operator <<(ostream& ros, Feature manipulator) {
    return manipulator(ros);
  }

private:
  ostream& operator()(ostream& ros) {
    ros << "[" << type << "," << descriptor.rows << "," << descriptor.cols << "]";
    return ros;
  }
};

#endif

