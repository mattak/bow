#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include <fstream>
#include "feature.h"
#include "book.h"

using namespace std;
using namespace cv;

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

	f1.extract("surf", "/home/maruya-t/git/bow/72.jpg");
	f2.extract("surf", "/home/maruya-t/git/bow/72m.jpg");

	Book book;
	book.add(f1);
	book.add(f2);
	cout << "added" << endl;
	
	book.makebook(100);
	cout << "makebook" << endl;
	cout << book.book.rows << "," << book.book.cols << endl;
	book.maketree(TYPE_KDTREE);
	cout << "maketree" << endl;
	flann::KDTreeIndexParams ip;
	flann::Index idx(book.book, ip);
	idx.save("tmp.tree");
	cout << "tree saved" << endl;
	book.save_book("tmp.book");
	cout << "saved" << endl;
	
	Mat indices;
	Mat dists;
	flann::SearchParams p;
	book.tree.knnSearch(book.book, indices, dists, 10, p );
	
	Book loadbook;
	loadbook.load_book("tmp.book");
	loadbook.tree.load(loadbook.book, "tmp.tree");
	//loadbook.load("tmp.book", "tmp.tree");
	
	return 0;
}

