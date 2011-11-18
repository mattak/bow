#ifndef __HEADER_BOOK
#define __HEADER_BOOK

#include <cv.h>
#include <cxcore.h>
#include <vector>
#include <fstream>

#include "feature.h"

using namespace cv;
using namespace std;

class Book {
public:
  Book();
  ~Book();
  
  vector<Feature> features;
  Mat book;
  
  void makebook(int k);

  void add (const char *file, const bool bin=false);
  void add (Feature& f);

  void save (const char *file);
  void load (const char *file);

  void write (ofstream& ofs);
  void read (ifstream& ifs);
};

#endif

