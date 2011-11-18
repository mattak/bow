#include <cv.h>
#include "book.h"
#include "util.h"

Book::Book(){
  
}
Book::~Book(){

}

void Book::makebook (int k) {
  Mat points;
  int rows=0, cols=0;
  for (int i=0;i<features.size();i++) {
    rows += features.at(i).descriptor.rows;
    cols = features.at(i).descriptor.cols;
  }
  
  points = Mat::zeros(rows,cols,CV_32FC1);
  int sy = 0;
  for (int i=0;i<features.size();i++) {
    Feature f = features.at(i);
    for (int y=0;y<f.descriptor.rows;y++,sy++) {
      for (int x=0;x<f.descriptor.cols;x++) {
        points.at<float>(sy,x) = f.descriptor.at<float>(y,x);
      }
    }
  }
  Mat label;
  // for
  kmeans(
    points,
    k,
    label,
    cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),
    1,
    KMEANS_PP_CENTERS,
    book
  );
  
}

void Book::add (const char *file, bool bin) {
  Feature f;
  ifstream ifs;
  char c;
  if (bin) {
    ifs.open(file, ios::in|ios::binary);
    while(true){
      ifs.read((char*)&c, sizeof(char));
      if (c=='^') {
        Feature f;
        f.bread(ifs);
        features.push_back(f);
      }
      else {
        break;
      }
    }
  }
  else {
    ifs.open(file, ios::in);
    while (true) {
      ifs.read((char*)&c, sizeof(char));
      if (c=='^') {
        Feature f;
        f.read(ifs);
        features.push_back(f);
      }
      else {
        break;
      }
    }
  }
  
}

void Book::add (Feature& f) {
  features.push_back(f);
}

void Book::save (const char* file) {
  ofstream ofs;
  ofs.open(file, ios::out);
  write(ofs);
  ofs.close();
}

void Book::load (const char* file) {
  ifstream ifs;
  ifs.open(file, ios::in);
  read(ifs);
  ifs.close();
}

void Book::write (ofstream& ofs) {
  ofs << "^" << endl;
  ofs << "type book" << endl;
  ofs << "feature " << features.at(0).type << endl;
  ofs << "size " << book.rows << " " << book.cols << endl;
  ofs << "$" << endl;
  for (int y=0; y<book.rows; y++) {
    ofs << book.at<float>(y,0);
    for (int x=1; x<book.cols; x++) {
      ofs << " " << book.at<float>(y,x);
    }
    ofs << endl;
  }
}

void Book::read (ifstream& ifs) {
  int y=0, x=0;
  bool header = false;
  string line;
  while (ifs && getline(ifs,line)) {
    if (line == "^") {
      header = true;
      continue;
    }
    else if(line == "$") {
      header = false;
      continue;
    }
    if (header) {
      vector<string> strs = split(line, " ");
      string tag = strs.at(0);
      if (tag=="type") {
        ;//type = strs.at(1);
      }
      else if (tag=="size") {
        int rows = atoi(strs.at(1).c_str());
        int cols = atoi(strs.at(2).c_str());
        book = Mat::zeros(rows,cols,CV_32FC1);
      }
    }
    else {
      vector<string> strs = split(line, " ");
      for (int x=0;x<strs.size();x++) {
        float f = atof(strs.at(x).c_str());
				book.at<float>(y,x) = f;
      }
      y++;
    }
  }
}

