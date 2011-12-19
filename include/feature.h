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
  bool io_binary;
public:
	enum DataType {FLOAT, UCHAR};

	string type;
	Mat descriptor;
	//vector<KeyPoint> keypoint;
	
	// constractor
  Feature() { io_binary = true; }
	~Feature() {;}

	virtual void add_grid(int wstep, int hstep, int scale) = 0;

	void extract(const char *type, const char *imgfile);
  virtual void extract(const char *type, Mat& grayimg) = 0;
	
	//void show();

	// io
  void set_io_binary(bool bin);
	void save(const char* file, const bool bin=true);
	void load(const char* file);
	//void write(ofstream& ofs, const bool bin=true);
	//bool read(ifstream& ifs);
  virtual ostream& write(ostream& os, const bool bin=true) = 0;
  virtual bool read(istream& is) = 0;

	// util
	Mat floated_descriptor();
	virtual DataType data_type() = 0;

  friend ostream& operator <<(ostream &os, Feature& manipulator) {
    return manipulator.write(os, manipulator.io_binary);
  }
  friend istream& operator >>(istream &is, Feature& manipulator) {
    manipulator.read(is);
    return is;
  }
/*
  virtual ostream& operator()(ostream& os);
  virtual istream& operator()(istream& is);

  friend ostream& operator <<(ostream& ros, Feature manipulator) {
    return manipulator(ros);
  }

private:
  ostream& operator()(ostream& ros) {
    ros << "[" << type << "," << descriptor.rows << "," << descriptor.cols << "]";
    return ros;
  }
*/
};

class KeyPointFeature : public Feature {
public:
  KeyPointFeature() {}
  ~KeyPointFeature() {}

  vector<KeyPoint> keypoint;
  void add_grid(int wstep, int hstep, int scale);
  void extract(const char *type, const char *imgpath) {
    Feature::extract(type,imgpath);
  }
  void extract(const char *type, Mat& grayimg);
  void show();

	DataType data_type();
  ostream& write(ostream& os, const bool bin=true);
  bool read(istream& is);
};

/*
class ColorFeature : public Feature {
public:
  void add_grid(int wstep, int hstep, int scale=0);
  void extract(Mat& img);
}
*/

namespace FeatureFactory {
  //FeatureFactory() {}
  //~FeatureFactory() {}
  vector< Ptr<Feature> > loadFeatures(const char *file);
  vector< Ptr<Feature> > loadFeatures(istream& is);
  Ptr<Feature> loadFeature(const char *file);
  Ptr<Feature> loadFeature(istream& is);
  
};

#endif

