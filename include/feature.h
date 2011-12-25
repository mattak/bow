#ifndef __HEADER_FEATURE
#define __HEADER_FEATURE

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "util.h"

#define FEATURE_TYPE_SURF "surf"
#define FEATURE_TYPE_SIFT "sift"
#define FEATURE_TYPE_ORB  "orb"

using namespace std;
using namespace cv;

class Feature {
protected:
  HeaderInfo headerinfo;
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

	virtual void extract(const char *type, const char *imgfile) = 0;
  virtual void extract(const char *type, Mat& grayimg) = 0;
	
	virtual void show() = 0;

	// io
  void set_io_binary(bool bin);
	void save(const char* file, const bool bin=true);
	void load(const char* file);

  virtual ostream& write(ostream& os, const bool bin=true) = 0;
  bool read(istream& is);
  virtual bool read(istream& is, HeaderInfo& info) = 0;

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
};

class KeyPointFeature : public Feature {
public:
  KeyPointFeature() {}
  ~KeyPointFeature() {}
  
  void init();

  vector<KeyPoint> keypoint;

  void add_grid(int wstep, int hstep, int scale);
  void extract(const char *type, const char *imgpath) {
    Mat img = imread(imgpath, 0);
    loadfile = string(imgpath);
    if (img.empty() || img.rows<=0 || img.cols<=0) {
      cerr << "error loading image:" << loadfile << endl;
      return;
    }
    extract(type,img);
  }
  void extract(const char *type, Mat& grayimg);
  void show();

	DataType data_type();
  ostream& write(ostream& os, const bool bin=true);
  bool read(istream& is, HeaderInfo& info);
};

class ColorPatchFeature : public Feature {
public:
  ColorPatchFeature() {}
  ~ColorPatchFeature() {}

  vector<KeyPoint> keypoint;
  void init();

  void add_grid(int wstep, int hstep, int scale=0);
  void extract(const char *type, const char *imgpath) {
    Mat img = imread(imgpath,1);
    loadfile = string(imgpath);
    if (img.empty() || img.rows<=0 || img.cols<=0) {
      cerr << "error loading image:" << loadfile << endl;
      return;
    }
    extract(type,img);
  }
  void extract(const char *type, Mat& img);
  void show();
  
  DataType data_type();
  ostream& write(ostream& os, const bool bin=true);
  bool read (istream& is, HeaderInfo& info);

private:
  void extract(const Mat& img, Mat& colorhist, const int wgrid, const int hgrid);
};

namespace FeatureFactory {
  vector< Ptr<Feature> > loadFeatures(const char *file);
  vector< Ptr<Feature> > loadFeatures(istream& is);
  Ptr<Feature> loadFeature(const char *file);
  Ptr<Feature> loadFeature(istream& is);
};

#endif

