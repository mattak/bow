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

void test_book_saveload(Feature& f1, Feature& f2) {
	Book book;
	book.add(f1);
	book.add(f2);
	book.makebook(100);
	book.save_book("tmp_test.book");
	
	Book lbook;
	lbook.load_book("tmp_test.book");
	cout << "sbook" << (book.book.at<float>(book.book.rows-1,book.book.cols-1)) << endl;
	cout << "lbook" << (lbook.book.at<float>(lbook.book.rows-1,lbook.book.cols-1)) << endl;
}

void test_index_saveload(Feature& f1, Feature& f2) {
	Book book;
	book.add(f1);
	book.add(f2);
	book.makebook(100);
	// save
	flann::KDTreeIndexParams kdp;
	flann::Index sidx(book.book, kdp);
	sidx.save("tmp_test.idx");
	
	// load
	flann::Index lidx;//(book.book,kdp);
	lidx.load(book.book, "tmp_test.idx");
	
	// test
	Mat query = Mat::ones(1,book.book.cols,CV_32FC1);
	flann::SearchParams searchparams;
	Mat sindices;
	Mat sdists;
	sidx.knnSearch(query, sindices, sdists, 10, searchparams);
	cout << "before indices:" << sindices << endl;
	cout << "before dists  :" << sdists << endl;
	Mat lindices;
	Mat ldists;
	lidx.knnSearch(query, lindices, ldists, 10, searchparams);
	cout << "after indices:" << lindices << endl;
	cout << "after dists  :" << ldists << endl;
  cout << "book:" << book << endl;
}


// feature -> book
// book -> words
// 
int main(int argc, char** argv){
	Feature f1;
	Feature f2;

	f1.extract("surf", "/home/maruya-t/git/bow/67.jpg");
	f2.extract("surf", "/home/maruya-t/git/bow/67m.jpg");
	
	// test_book_saveload(f1,f2);
	test_index_saveload(f1,f2);

	return 0;
}

