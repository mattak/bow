#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "bow.h"

using namespace cv;
using namespace std;

Feature::Feature(){

}
Feature::~Feature(){

}


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

// 
void Feature::extract (const char *_type, const char *_imgfile) {
	string s(_type);
	string f(_imgfile);
	type = s;
	loadfile = f;
	Mat img = imread(_imgfile, 0);
	
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
	else if (type == "gsurf") {
		extractor = new SurfDescriptorExtractor();
	}
	else if (type == "gsift") {
		extractor = new SiftDescriptorExtractor();
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
void Feature::add_grid (int wstep, int hstep, int scale) {
	xgrids.push_back(wstep);
	ygrids.push_back(hstep);
	sgrids.push_back(scale);
}

// show
void Feature::show() {
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
void Feature::save (const char* file, const bool bin) {
	ofstream ofs;
	if (bin) {
		ofs.open(file, ios::out|ios::binary);
		bwrite(ofs);
	}
	else {
		ofs.open(file, ios::out);
		write(ofs);
	}
	ofs.close();
}

void Feature::load (const char* file, const bool bin) {
	ifstream ifs;
	if (bin) {
		ifs.open(file, ios::in|ios::binary);
		bread(ifs);
	}
	else {
		ifs.open(file, ios::in);
		read(ifs);
	}
	ifs.close();
}


// stream writer, reader
void Feature::read (ifstream& ifs) {
	int y=0, x=0;
	bool header=false;
	string line;
	while (ifs && getline(ifs,line)) {
		if (line == "^") {
			header = true;
			continue;
		}
		if (line == "$") {
			header = false;
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
		}
		else {
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
		}
	}
}

void Feature::write (ofstream& ofs) {
	ofs << "^" << endl;
	ofs << "type " << type << endl;
	ofs << "size " << descriptor.rows << " " << descriptor.cols << endl;
	ofs << "loadfile " << loadfile << endl;
	ofs << "$" << endl;
	for (int y=0; y<descriptor.rows; y++) {
		KeyPoint kp = keypoint.at(y);
		ofs << kp.pt.x << " " << kp.pt.y << " ";
		ofs << kp.size << " " << kp.angle;
		for (int x=0; x<descriptor.cols; x++) {
			ofs << " " << descriptor.at<float>(y,x);
		}
		ofs << endl;
	}
}

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
		for (int x=0;x<descriptor.cols;x++) {
			float d;
			ifs.read((char*)&d, sizeof(float));
			descriptor.at<float>(y,x) = d;
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
		for(int x=0;x<descriptor.cols;x++) {
			float d = descriptor.at<float>(y,x);
			ofs.write((char*)&d, sizeof(float));
		}
	}
}

