#ifndef __HEADER_BOOK
#define __HEADER_BOOK

#include <cv.h>
#include <cxcore.h>
#include <vector>
#include <fstream>

#include "feature.h"
#include "util.h"

using namespace cv;
using namespace std;

#define TYPE_KDTREE 0
#define TYPE_KMTREE 1
#define TYPE_LLTREE 2

class Book : public DataIO {
public:
  Book() {;}
  ~Book() {;}
  
	vector< Ptr<Feature> > features;
	Mat book;
  
  int  makebook(int k, const int hierarchical_level=1, const int maxfeaturesize=100000);

	void getword(Ptr<Feature> &f, Mat& dst_word, flann::Index& idx, const int knn=3);
	void getword(Mat& src, Mat& dst_word, flann::Index& idx, const int knn=3);

	void getwords (vector< Ptr<Feature> >& fs, Mat& dst_words, const int knn=3);
	void getwords (const char* file, Mat& dst_words, const int knn=3);

  void add (const char *file);
  void add (Ptr<Feature> &f);

  //void save_book (const char *file, const bool bin=true);
  //void load_book (const char *file);

  ostream& write (ostream& os, const bool bin=true);
  bool read (istream& is, HeaderInfo& info);
};

int hierarchical_kmeans (int k, Mat& points, Mat& label, Mat& cluster, int level);
int balanced_kmeans (int k, Mat& points, Mat& label, Mat& cluster);

#endif

