#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "feature.h"
#include "util.h"

#define rep(i,n) for(int i=0;i<n;i++)

using namespace cv;
using namespace std;

Feature::DataType KeyPointFeature::data_type() {
	if (type=="orb" || type=="gorb") {
		return Feature::UCHAR;
	}
	return Feature::FLOAT;
}

Feature::DataType ColorPatchFeature::data_type() {
  return Feature::FLOAT;
}

void KeyPointFeature::init() {
  keypoint.clear();
}

void ColorPatchFeature::init() {
  keypoint.clear();
}

Mat Feature::floated_descriptor() {
	if (data_type()==Feature::UCHAR) {
		Mat dsc = Mat::zeros(descriptor.rows,descriptor.cols*8,CV_32FC1);
		for (int y=0;y<descriptor.rows;y++) {
			for (int x=0;x<descriptor.cols;x++) {
				uchar c = descriptor.at<uchar>(y,x);
				for (int p=0;p<8;p++) {
					int v = (c >> p) & 0x01;
					dsc.at<float>(y,x*8+p) = (float)v;
				}
			}
		}
		return dsc;
	}
	return descriptor;
}

void ColorPatchFeature::extract (const char *_type, Mat& colorimg) {
  if (colorimg.channels()!=3) {
    cerr << "illeagal channel size" << endl;
    return;
  }
  type = string(_type);
  Mat dstimg = colorimg;

  string subtype = (type.at(0)=='g') ? type.substr(1) : type;

  if (subtype == "lab") {
    cvtColor(colorimg, dstimg, CV_BGR2Lab);
  }
  else if (subtype == "hsv") {
    cvtColor(colorimg, dstimg, CV_BGR2HSV);
  }
  else if (subtype == "ycrcb") {
    cvtColor(colorimg, dstimg, CV_BGR2YCrCb);
  }
	if (type.at(0)!='g') {
		extract(dstimg, descriptor, 1, 1);
	}
  if (!xgrids.empty()) {
    rep (i,xgrids.size()) {
      int wgrid = xgrids.at(i);
      int hgrid = ygrids.at(i);
      Mat tmp;
      extract(dstimg, tmp, wgrid, hgrid);
      descriptor.push_back(tmp);
    }
  }
}

void ColorPatchFeature::extract (const Mat& img, Mat& colorhist, const int wgrid, const int hgrid) {
  int wstep = img.cols / wgrid;
  int hstep = img.rows / hgrid;
  int bin[3] = {8,8,8};
  int binstep[3] = {256/bin[0], 256/bin[1], 256/bin[2]};
  colorhist = Mat::zeros(wgrid*hgrid,bin[0]+bin[1]+bin[2],CV_32FC1);
  
  rep(yg,hgrid) {
    rep(y,hstep) {
      rep(xg,wgrid) {
        rep(x,wstep) {
          int ay = yg*hstep+y;
          int ax = xg*wstep+x;
          int a  = ay*img.step + ax*3;
          int e[3] = {(uchar)img.data[a],
                      (uchar)img.data[a+1],
                      (uchar)img.data[a+2]};
          e[0] = e[0]/binstep[0];
          e[1] = e[1]/binstep[1] + bin[0];
          e[2] = e[2]/binstep[2] + bin[0] + bin[1];
          colorhist.at<float>(yg*wgrid+xg, e[0]) += 1.0f;
          colorhist.at<float>(yg*wgrid+xg, e[1]) += 1.0f;
          colorhist.at<float>(yg*wgrid+xg, e[2]) += 1.0f;
        }
      }
    }
  }
  
  // normalize
  rep(i,colorhist.rows) {
    rep(j,colorhist.cols) {
      colorhist.at<float>(i,j) /= (wstep*hstep);
    }
    KeyPoint kp;
    int x = i % wgrid;
    int y = i / wgrid;
    kp.pt.x  = wstep*x+wstep/2;
    kp.pt.y  = hstep*y+hstep/2;
    kp.size = (wstep<hstep) ? wstep/2 : hstep/2;
    keypoint.push_back(kp);
  }
}

void KeyPointFeature::extract (const char *_type, Mat& img) {
	string s(_type);
	type = s;
	
	// add grid points
	keypoint.clear();
	
	Ptr<FeatureDetector> detector;
	Ptr<DescriptorExtractor> extractor;
	if (type == "surf") {
		detector = new SurfFeatureDetector(400);
		extractor = new SurfDescriptorExtractor();
		detector->detect(img, keypoint);
	}
	else if (type == "sift") {
		detector = new SiftFeatureDetector(0.01, 10.0);
		extractor = new SiftDescriptorExtractor();
		detector->detect(img, keypoint);
	}
	else if (type == "orb") {
		detector = new OrbFeatureDetector(1000);
		extractor = new OrbDescriptorExtractor();
		detector->detect(img, keypoint);
	}
	else if (type == "gsurf") {
		extractor = new SurfDescriptorExtractor();
	}
	else if (type == "gsift") {
		extractor = new SiftDescriptorExtractor();
	}
	else if (type == "gorb") {
		extractor = new OrbDescriptorExtractor();
	}
	else {
		cerr << "unknown type of descriptor" << endl;
		return;
	}

	for (int i=0;i<xgrids.size();i++){
		int wstep = xgrids.at(i);
		int hstep = ygrids.at(i);
		int scale = sgrids.at(i);
		for (int y=hstep/2;y<img.rows;y+=hstep) {
			for (int x=wstep/2;x<img.cols;x+=wstep) {
				KeyPoint kp(x, y, scale);
				keypoint.push_back(kp);
			}
		}
	}

	extractor->compute(img, keypoint, descriptor);
}

// 
void KeyPointFeature::add_grid (int wstep, int hstep, int scale) {
	xgrids.push_back(wstep);
	ygrids.push_back(hstep);
	sgrids.push_back(scale);
}

void ColorPatchFeature::add_grid (int wstep, int hstep, int scale) {
	xgrids.push_back(wstep);
	ygrids.push_back(hstep);
	sgrids.push_back(scale);
}

// show
void KeyPointFeature::show() {
  if (loadfile.empty()) {
    cerr << "empty loadpath" << endl;
    return;
  }
	Mat canvas = imread(loadfile,1);

	Scalar color(255,0,0);
	for(int i=0;i<keypoint.size();i++){
		KeyPoint kp = keypoint.at(i);
		circle(canvas, kp.pt, kp.size, color, 1, CV_AA);
	}
	namedWindow("canvas",CV_WINDOW_AUTOSIZE);
	imshow("canvas", canvas);
	waitKey(0);
}

void ColorPatchFeature::show() {
  if (loadfile.empty()) {
    cerr << "empty loadpath" << endl;
    return;
  }
	Mat canvas = imread(loadfile,1);

	for(int i=0;i<keypoint.size();i++){
		KeyPoint kp = keypoint.at(i);
    int maxbgr[3] = {0,0,0};
    for (int k=0;k<3;k++) {
      float max = 0;
      for (int j=0;j<8;j++) {
        float d = descriptor.at<float>(i,8*k+j);
        if (d > max){
          max = d;
          maxbgr[k] = j*64+32;
        }
      }
    }
    Scalar color = CV_RGB(maxbgr[2],maxbgr[1],maxbgr[0]);
    Scalar rcolor;
    if (maxbgr[0]>=240 && maxbgr[1]>=240 && maxbgr[0]>=240) {
      rcolor = CV_RGB(0,0,0);
    }
    else{
      rcolor = CV_RGB(255,255,255);
    }
    circle(canvas, kp.pt, kp.size, rcolor, 3, CV_AA);
		circle(canvas, kp.pt, kp.size, color, 2, CV_AA);
	}
	namedWindow("canvas",CV_WINDOW_AUTOSIZE);
	imshow("canvas", canvas);
	waitKey(0);
}

// file writer, reader
void Feature::set_io_binary (bool bin) {
  io_binary = bin;
}

void Feature::save (const char* file, const bool bin) {
	ofstream ofs;
	ofs.open(file, ios::out|ios::binary);
	write(ofs, bin);
	ofs.close();
}

void Feature::load (const char* file) {
	ifstream ifs;
	ifs.open(file, ios::in|ios::binary);
	read(ifs);
	ifs.close();
}

// stream writer, reader
ostream& KeyPointFeature::write(ostream& os, const bool bin) {
	os << "^" << endl;
	os << "type " << type << endl;
	os << "size " << descriptor.rows << " " << descriptor.cols << endl;
	os << "loadfile " << loadfile << endl;
	os << "binary " << bin << endl;
	os << "$" << endl;
	if (bin) {
		rep (y,descriptor.rows) {
			KeyPoint kp = keypoint.at(y);
			float px = kp.pt.x;
			float py = kp.pt.y;
			float s = kp.size;
			float r = kp.angle;
			os.write((char*)&px,sizeof(float));
			os.write((char*)&py,sizeof(float));
			os.write((char*)&s,sizeof(float));
			os.write((char*)&r,sizeof(float));
			if (type=="orb" || type=="gorb") {
				rep (x,descriptor.cols) {
					uchar c = descriptor.at<uchar>(y,x);
					os.write((char*)&c, sizeof(uchar));
				}
			}
			else {
				rep (x,descriptor.cols) {
					float f = descriptor.at<float>(y,x);
					os.write((char*)&f, sizeof(float));
				}
			}
		}
	}
	else {
		rep (y,descriptor.rows) {
			KeyPoint kp = keypoint.at(y);
			os << kp.pt.x << " " << kp.pt.y << " ";
			os << kp.size << " " << kp.angle;
			if (type=="orb" || type=="gorb") {
				os << " ";
				rep (x,descriptor.cols) {
					uchar c = descriptor.at<uchar>(y,x);
					for (int p=7;p>=0;p--) {
						int v = (c >> p) & 0x01;
						os << v;
					}
				}
			}
			else {
				rep (x,descriptor.cols) {
					os << " " << descriptor.at<float>(y,x);
				}
			}
			os << endl;
		}
	}
  return os;
}

ostream& ColorPatchFeature::write(ostream& os, const bool bin) {
	os << "^" << endl;
	os << "type " << type << endl;
	os << "size " << descriptor.rows << " " << descriptor.cols << endl;
	os << "loadfile " << loadfile << endl;
	os << "binary " << bin << endl;
	os << "$" << endl;
	if (bin) {
		rep (y,descriptor.rows) {
			KeyPoint kp = keypoint.at(y);
			float px = kp.pt.x;
			float py = kp.pt.y;
			float s = kp.size;
			float r = kp.angle;
			os.write((char*)&px,sizeof(float));
			os.write((char*)&py,sizeof(float));
			os.write((char*)&s,sizeof(float));
			os.write((char*)&r,sizeof(float));
			rep (x,descriptor.cols) {
				float f = descriptor.at<float>(y,x);
				os.write((char*)&f, sizeof(float));
			}
		}
	}
	else {
		rep (y,descriptor.rows) {
			KeyPoint kp = keypoint.at(y);
			os << kp.pt.x << " " << kp.pt.y << " ";
			os << kp.size << " " << kp.angle;
			rep (x,descriptor.cols) {
				os << " " << descriptor.at<float>(y,x);
			}
			os << endl;
		}
	}
  return os;
}

bool Feature::read (istream& is) {
  HeaderInfo info;
  is >> info;
  if (info.data.empty()) {
    return false;
  }
  return read(is, info);
}

bool KeyPointFeature::read(istream& is, HeaderInfo& info) {
	bool read_binary;
	bool done=false;
	string line;
  {
    read_binary = (info.data["binary"].front()=="1")? true : false;
    type     = info.data["type"][0];
    loadfile = info.data["loadfile"][0];
    int rows = atoi(info.data["size"][0].c_str());
    int cols = atoi(info.data["size"][1].c_str());
    if (type=="orb" || type=="gorb") {
      descriptor = Mat::zeros(rows, cols, CV_8UC1);
    } else {
      descriptor = Mat::zeros(rows, cols, CV_32FC1);
    }
  }
  // initialize
  init();
  
	if (read_binary) {
		rep (y,descriptor.rows) {
			float px, py, scale, angle;
			is.read((char*)&px,sizeof(float));
			is.read((char*)&py,sizeof(float));
			is.read((char*)&scale,sizeof(float));
			is.read((char*)&angle,sizeof(float));
			KeyPoint kp(px,py,scale,angle);
			keypoint.push_back(kp);
			if (type=="orb" || type=="gorb") {
				rep (x,descriptor.cols) {
					uchar c;
					is.read((char*)&c, sizeof(uchar));
					descriptor.at<uchar>(y,x) = c;
				}
			}
			else {
				rep (x,descriptor.cols) {
					float f;
					is.read((char*)&f, sizeof(float));
					descriptor.at<float>(y,x) = f;
				}
			}
		}
		done = true;
	}
	else {
		// read plain
    int y = 0;
    while (!done && is && getline(is,line)) {
		  vector<string> strs = split(line, " ");
		  float px, py, scale, angle;
		  px = (float)atof(strs.at(0).c_str());
		  py = (float)atof(strs.at(1).c_str());
		  scale = (float)atof(strs.at(2).c_str());
		  angle = (float)atof(strs.at(3).c_str());
		  KeyPoint kp(px,py,scale,angle);
		  keypoint.push_back(kp);
		  if (type=="orb" || type=="gorb") {
			  const char *vals = strs.at(4).c_str();
			  rep (x,descriptor.cols) {
				  uchar c = 0;
				  for (int p=0;p<8;p++) {
					  c = c << 1;
					  if (vals[x*8+p] == '1') {
						  c += 1;
					  }
				  }
				  descriptor.at<uchar>(y,x) = c;
			  }
		  }
		  else {
			  for (int x=4;x<strs.size();x++) {
				  float f = atof(strs.at(x).c_str());
				  descriptor.at<float>(y,x-4) = f;
			  }
		  }
		  y++;
		  if (y>=descriptor.rows) done = true;
    }
	}
  if (!done) {
    cerr << "reading data false" << endl;
  }
	return done;
}

bool ColorPatchFeature::read (istream& is, HeaderInfo& info) {
	bool read_binary;
	bool done=false;
	string line;
  {
    read_binary = (info.data["binary"].front()=="1")? true : false;
    type     = info.data["type"][0];
    loadfile = info.data["loadfile"][0];
    int rows = atoi(info.data["size"][0].c_str());
    int cols = atoi(info.data["size"][1].c_str());
    descriptor = Mat::zeros(rows, cols, CV_32FC1);
  }
  // initialize
  init();
  
	if (read_binary) {
		rep (y,descriptor.rows) {
			float px, py, scale, angle;
			is.read((char*)&px,sizeof(float));
			is.read((char*)&py,sizeof(float));
			is.read((char*)&scale,sizeof(float));
			is.read((char*)&angle,sizeof(float));
			KeyPoint kp(px,py,scale,angle);
			keypoint.push_back(kp);
			rep (x,descriptor.cols) {
				float f;
				is.read((char*)&f, sizeof(float));
				descriptor.at<float>(y,x) = f;
			}
		}
		done = true;
	}
	else {
		// read plain
    int y = 0;
    while (!done && is && getline(is,line)) {
		  vector<string> strs = split(line, " ");
		  float px, py, scale, angle;
		  px = (float)atof(strs.at(0).c_str());
		  py = (float)atof(strs.at(1).c_str());
		  scale = (float)atof(strs.at(2).c_str());
		  angle = (float)atof(strs.at(3).c_str());
		  KeyPoint kp(px,py,scale,angle);
		  keypoint.push_back(kp);
			for (int x=4;x<strs.size();x++) {
			  float f = atof(strs.at(x).c_str());
			  descriptor.at<float>(y,x-4) = f;
			}
		  y++;
		  if (y>=descriptor.rows) done = true;
    }
	}
  if (!done) {
    cerr << "reading data false" << endl;
  }
	return done;
}

vector< Ptr<Feature> > FeatureFactory::loadFeatures(const char *file) {
  ifstream ifs;
  ifs.open(file, ios::in | ios::binary);
  vector< Ptr<Feature> > features;
  features = loadFeatures(ifs);
  ifs.close();
  return features;
}

vector< Ptr<Feature> > FeatureFactory::loadFeatures(istream& is) {
  vector< Ptr<Feature> > features;
  while (is && !is.eof()) {
    Ptr<Feature> f = loadFeature(is);
    if (f.empty()) {
      break;
    }
    features.push_back(f);
  }
  return features;
}

Ptr<Feature> FeatureFactory::loadFeature(const char *file) {
  ifstream ifs;
  ifs.open(file, ios::in | ios::binary);
  Ptr<Feature> f = loadFeature(ifs);
  ifs.close();
  return f;
}

Ptr<Feature> FeatureFactory::loadFeature(istream& is) {
  // find type
  Ptr<Feature> feature;
  HeaderInfo info;
  is >> info;
  if (info.data.empty()) {
    return feature;
  }
  string type = info.data["type"][0];
  if (type=="orb" || type=="gorb" ||
      type=="surf" || type=="gsurf" ||
      type=="sift" || type=="gsift") {
    feature = new KeyPointFeature();
    feature->read(is, info);
  }
  else if (type=="rgb" || type=="grgb"
			|| type=="lab"	 || type=="glab"
			|| type=="hsv"   || type=="ghsv"
			|| type=="ycrcb" || type=="gycrcb") {
    feature = new ColorPatchFeature();
    feature->read(is, info);
  }
  else {
    cerr << "unknown feature type" << endl;
    exit(0);
  }
  return feature;
}

