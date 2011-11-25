#ifndef __HEADER_BOOK
#define __HEADER_BOOK

#include <cv.h>
#include <cxcore.h>
#include <vector>
#include <fstream>

#include "feature.h"

using namespace cv;
using namespace std;

#define TYPE_KDTREE 0
#define TYPE_KMTREE 1
#define TYPE_LLTREE 2

class Book {
public:
  Book();
  ~Book();
  
	vector<Feature> features;
	Mat book;
//	cv::flann::Index tree;
  
  int  makebook(int k, const bool hierarchical=false);
//  void maketree(int type);

	void getword(const Feature &f, Mat& dst_word);
	void getword(const Mat& src, Mat& dst_word);

  void add (const char *file, const bool bin=false);
  void add (Feature& f);

//  void save (const char *bookfile, const char* treefile);
//  void load (const char *bookfile, const char* treefile);

  void save_book (const char *file, const bool bin=true);
  void load_book (const char *file);

  void write_book (ofstream& ofs, const bool bin=true);
  void read_book (ifstream& ifs);
	
	friend ostream& operator <<(ostream& ros, Book manipulator) {
		return manipulator(ros);
	}

private:
	ostream& operator()(ostream& ros) {
		ros << "[row,col]=[" << book.rows << "," << book.cols << "]" << endl;
		return ros;
	}
};

#endif

