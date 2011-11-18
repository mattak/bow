#include <iostream>
#include "feature.h"
#include "book.h"

using namespace std;

void save(const char *file){
  Feature f;
  f.add_grid(20,20,10);
  //f.extract("surf", `any jpg`);
  f.save(file);
  cout << "last:" << f.descriptor.at<float>(f.descriptor.rows-1,f.descriptor.cols-1) << endl;
  cout << "klast:" << f.keypoint.at(f.keypoint.size()-1).pt.x << endl;
  f.show();
}

void load(const char* file){
  Feature f;
  f.load(file);
  cout << "last:" << f.descriptor.at<float>(f.descriptor.rows-1,f.descriptor.cols-1) << endl;
  cout << "klast:" << f.keypoint.at(f.keypoint.size()-1).pt.x << endl;
  f.show();
}

int main(int argc, char** argv){
	Feature f1;
	Feature f2;
	return 0;
}

