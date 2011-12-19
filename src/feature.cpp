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

void extract_color_feature (const Mat& img, Mat& colorhist, const int wgrid, const int hgrid) {
  int wstep = img.cols / wgrid;
  int hstep = img.rows / hgrid;
  int bin[3] = {8,8,8};
  int binstep[3] = {256/bin[0], 256/bin[1], 256/bin[2]};
  colorhist = Mat::zeros(wgrid*hgrid,bin[0]*bin[1]*bin[2],CV_32FC1);
  
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
          printf("e %03d %03d %03d\n", e[0], e[1], e[2]);
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
  }
  
}

void Feature::extract (const char *_type, const char* imgfile) {
  Mat img = imread(imgfile, 0);
  string f(imgfile);
  loadfile = f;
  extract(_type, img);
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

// show
void KeyPointFeature::show() {
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
			float x = kp.pt.x;
			float y = kp.pt.y;
			float s = kp.size;
			float r = kp.angle;
			os.write((char*)&x,sizeof(float));
			os.write((char*)&y,sizeof(float));
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

bool KeyPointFeature::read(istream& is) {
	int y=0, x=0;
	bool header=false;
	bool read_binary=true;
	bool readed_header = false;
	bool done=false;
	string line;
	while (!done && is && getline(is,line)) {
		if (line == "^") {
			header = true;
			continue;
		}
		else if (line == "$") {
			header = false;
			readed_header = true;
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
			continue;
		}
		if (header) {
			vector<string> strs = split(line, " ");
			string tag = strs.at(0);
			if (tag=="type") {
				type = strs.at(1);
			}
			else if (tag=="size") {
				int rows, cols;
				rows = atoi(strs.at(1).c_str());
				cols = atoi(strs.at(2).c_str());
				descriptor = Mat::zeros(rows,cols,CV_32FC1);
			}
			else if (tag=="loadfile") {
				loadfile = strs.at(1);
			}
			else if (tag=="binary") {
				if (strs.at(1)=="1") {
					read_binary = true;
				}
				else {
					read_binary = false;
				}
			}
		}
		else {
			// read plain
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
	return readed_header;
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
  while(is && !is.eof()) {
    Ptr<Feature> f = loadFeature(is);
    features.push_back(f);
  }
  return features;
}

Ptr<Feature> FeatureFactory::loadFeature(const char *file) {
  ifstream ifs;
  ifs.open(file, ios::in | ios::binary);
  Ptr<Feature> f = loadFeature(file);
  ifs.close();
  return f;
}

Ptr<Feature> FeatureFactory::loadFeature(istream& is) {
  istream::pos_type start = is.tellg();
  // find type
  string type;
  string line;
  bool header = false;
  while (is && getline(is,line)) {
    if (line=="^") {
      header = true;
      continue;
    }
    else if (line=="$") {
      break;
    }
		if (header) {
			vector<string> strs = split(line, " ");
			string tag = strs.at(0);
			if (tag=="type") {
				type = strs.at(1);
			}
		}
  }
  cout << "feature factory readtype : " << type << endl;
  is.seekg(start);
  
  Ptr<Feature> f;
  if (type=="orb" || type=="gorb" ||
      type=="surf" || type=="gsurf" ||
      type=="sift" || type=="gsift") {
    f = new KeyPointFeature();
    f->read(is);
  }
  else {
    cerr << "unknown feature type" << endl;
    exit(0);
  }
  return f;
}

/*
bool Feature::read (ifstream& ifs) {
	int y=0, x=0;
	bool header=false;
	bool read_binary=true;
	bool readed_header = false;
	bool done=false;
	string line;
	while (!done && ifs && getline(ifs,line)) {
		if (line == "^") {
			header = true;
			continue;
		}
		else if (line == "$") {
			header = false;
			readed_header = true;
			if (read_binary) {
				for (int y=0;y<descriptor.rows;y++) {
					float px, py, scale, angle;
					ifs.read((char*)&px,sizeof(float));
					ifs.read((char*)&py,sizeof(float));
					ifs.read((char*)&scale,sizeof(float));
					ifs.read((char*)&angle,sizeof(float));
					KeyPoint kp(px,py,scale,angle);
					keypoint.push_back(kp);
					if (type=="orb" || type=="gorb") {
						for (int x=0;x<descriptor.cols;x++) {
							uchar c;
							ifs.read((char*)&c, sizeof(uchar));
							descriptor.at<uchar>(y,x) = c;
						}
					}
					else {
						for (int x=0;x<descriptor.cols;x++) {
							float f;
							ifs.read((char*)&f, sizeof(float));
							descriptor.at<float>(y,x) = f;
						}
					}
				}
				done = true;
			}
			continue;
		}
		if (header) {
			vector<string> strs = split(line, " ");
			string tag = strs.at(0);
			if (tag=="type") {
				type = strs.at(1);
			}
			else if (tag=="size") {
				int rows, cols;
				rows = atoi(strs.at(1).c_str());
				cols = atoi(strs.at(2).c_str());
				descriptor = Mat::zeros(rows,cols,CV_32FC1);
			}
			else if (tag=="loadfile") {
				loadfile = strs.at(1);
			}
			else if (tag=="binary") {
				if (strs.at(1)=="1") {
					read_binary = true;
				}
				else {
					read_binary = false;
				}
			}
		}
		else {
			// read plain
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
				for (int x=0;x<descriptor.cols;x++) {
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
	return readed_header;
}

void Feature::write (ofstream& ofs, const bool bin) {
	ofs << "^" << endl;
	ofs << "type " << type << endl;
	ofs << "size " << descriptor.rows << " " << descriptor.cols << endl;
	ofs << "loadfile " << loadfile << endl;
	ofs << "binary " << bin << endl;
	ofs << "$" << endl;
	if (bin) {
		for (int y=0; y<descriptor.rows; y++) {
			KeyPoint kp = keypoint.at(y);
			float x = kp.pt.x;
			float y = kp.pt.y;
			float s = kp.size;
			float r = kp.angle;
			ofs.write((char*)&x,sizeof(float));
			ofs.write((char*)&y,sizeof(float));
			ofs.write((char*)&s,sizeof(float));
			ofs.write((char*)&r,sizeof(float));
			if (type=="orb" || type=="gorb") {
				for (int x=0; x<descriptor.cols; x++) {
					uchar c = descriptor.at<uchar>(y,x);
					ofs.write((char*)&c, sizeof(uchar));
				}
			}
			else {
				for (int x=0; x<descriptor.cols; x++) {
					float f = descriptor.at<float>(y,x);
					ofs.write((char*)&f, sizeof(float));
				}
			} 
		}
	}
	else {
		for (int y=0; y<descriptor.rows; y++) {
			KeyPoint kp = keypoint.at(y);
			ofs << kp.pt.x << " " << kp.pt.y << " ";
			ofs << kp.size << " " << kp.angle;
			if (type=="orb" || type=="gorb") {
				ofs << " ";
				for (int x=0; x<descriptor.cols; x++) {
					uchar c = descriptor.at<uchar>(y,x);
					for (int p=7;p>=0;p--) {
						int v = (c >> p) & 0x01;
						ofs << v;
					}
				}
			}
			else {
				for (int x=0; x<descriptor.cols; x++) {
					ofs << " " << descriptor.at<float>(y,x);
				}
			}
			ofs << endl;
		}
	}
}
/*
void Feature::bread (ifstream& ifs) {
	const int BUFSIZE = 4096;
	char buf[BUFSIZE];
	for (int i=0;i<BUFSIZE;i++) {
		ifs.read((char*)&buf[i], sizeof(char));
		if (buf[i]=='$') {
			ifs.read((char*)&buf[i+1], sizeof(char));
			buf[i+2]='\0';
			break;
		}
	}
	string header(buf);
	vector<string> lines = split(header,"\n");
	for (int i=0;i<lines.size();i++) {
		vector<string> strs = split(lines.at(i)," ");
		if (strs.at(0) == "type") {
			type = strs.at(0);
		}
		else if (strs.at(0) == "size") {
			int rows = atoi(strs.at(1).c_str());
			int cols = atoi(strs.at(2).c_str());
			descriptor = Mat::zeros(rows,cols,CV_32FC1);
		}
		else if (strs.at(0) == "loadfile") {
			loadfile = strs.at(1);
		}
	}
	
	for (int y=0;y<descriptor.rows;y++) {
		float px, py, scale, angle;
		ifs.read((char*)&px,sizeof(float));
		ifs.read((char*)&py,sizeof(float));
		ifs.read((char*)&scale,sizeof(float));
		ifs.read((char*)&angle,sizeof(float));
		KeyPoint kp(px,py,scale,angle);
		keypoint.push_back(kp);
		if (type=="orb" || type=="gorb") {
			for (int x=0; x<descriptor.cols; x++) {
				uchar d;
				ifs.read((char*)&d, sizeof(unsigned char));
				descriptor.at<uchar>(y,x) = d;
			}
		}
		else {
			for (int x=0; x<descriptor.cols; x++) {
				float d;
				ifs.read((char*)&d, sizeof(float));
				descriptor.at<float>(y,x) = d;
			}
		}
	}
}

void Feature::bwrite (ofstream& ofs) {
	stringstream sstr;
	sstr << "^" << endl;
  sstr << "type " << type << endl;
  sstr << "size " << descriptor.rows << " " << descriptor.cols << endl;
  sstr << "loadfile " << loadfile << endl;
  sstr << "$" << endl;
	string str = sstr.str();
	const char *header = str.c_str();
	for(int i=0;i<str.size();i++){
		ofs.write((const char*)&header[i], sizeof(char));
	}

	for(int y=0;y<descriptor.rows;y++) {
		KeyPoint kp = keypoint.at(y);
		float px = kp.pt.x;
		float py = kp.pt.y;
		float size = kp.size;
		float angle = kp.angle;
		ofs.write((char*)&px,sizeof(float));
		ofs.write((char*)&py,sizeof(float));
		ofs.write((char*)&size,sizeof(float));
		ofs.write((char*)&angle,sizeof(float));
		if (type=="orb" || type=="gorb") {
			// binary output
			for (int x=0; x<descriptor.cols; x++) {
				uchar c = descriptor.at<uchar>(y,x);
				ofs.write((char*)&c, sizeof(unsigned char));
			}
		}
		else {
			for (int x=0; x<descriptor.cols; x++) {
				float d = descriptor.at<float>(y,x);
				ofs.write((char*)&d, sizeof(float));
			}
		}
	}
}
*/
