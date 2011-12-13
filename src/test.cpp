#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include <fstream>
#include "feature.h"
#include "book.h"
#include "util.h"

using namespace std;
using namespace cv;

void save (const char *file) {
  Feature f;
  f.add_grid(20,20,10);
  //f.extract("surf", `any jpg`);
  f.save(file);
  cout << "last:" << f.descriptor.at<float>(f.descriptor.rows-1,f.descriptor.cols-1) << endl;
  cout << "klast:" << f.keypoint.at(f.keypoint.size()-1).pt.x << endl;
  f.show();
}

void load (const char* file) {
  Feature f;
  f.load(file);
  cout << "last:" << f.descriptor.at<float>(f.descriptor.rows-1,f.descriptor.cols-1) << endl;
  cout << "klast:" << f.keypoint.at(f.keypoint.size()-1).pt.x << endl;
  f.show();
}

void test_book_saveload (Feature& f1, Feature& f2) {
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

void test_index_saveload (Feature& f1, Feature& f2) {
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

void test_words (Feature& f1, Feature& f2) {
	Book book;
	book.add(f1);
	book.add(f2);
	book.makebook(10);
	
	flann::KDTreeIndexParams kdparams;
	flann::Index idx(book.book, kdparams);

	Mat dst;
	book.getword(f1, dst, idx, 3);
	cout << dst << endl;
	book.getword(f2, dst, idx, 3);
	cout << dst << endl;

	Mat dsts;
	vector<Feature> fs;
	fs.push_back(f1); fs.push_back(f2);

	book.getwords(fs, dsts, 3);
	put_as_libsvm(1, dsts, cout);
}

void test_book_multi_load (const char* file) {
	Book book;
	book.add(file);
	for (int i=0;i<book.features.size();i++) {
		book.features.at(i).show();
	}
}

void test_hierarchical_kmeans() {
  int rows = 4;
  int cols = 2;
  Mat testpoints = Mat::zeros(rows,cols,CV_32FC1);
  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols;j++) {
      testpoints.at<float>(i,j) = (float)rand()/RAND_MAX;
    }
  }
  Mat cluster;
  Mat label;
  int res = hierarchical_kmeans(2, testpoints, label, cluster, 2);
  /*kmeans(
      testpoints,
      2,
      label,
      cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),
      1,
      KMEANS_PP_CENTERS,
      cluster
  );
  int res = balanced_kmeans(5, testpoints, label, cluster);
  */
  cout << res << endl;
  cout << testpoints << endl;
  cout << cluster << endl;
  cout << cluster.rows << endl;
}

// feature -> book
// book -> words
// 
int main (int argc, char** argv) {
	test_hierarchical_kmeans();
  /*
  if (argc>1) {
		test_book_multi_load(argv[1]);
	}
	else {
		cerr << "usage <multi book file>" << endl;
		exit(0);
	}
	return 0;
	
	Feature f1;
	Feature f2;

	f1.extract("orb", "/home/maruya-t/git/bow/67.jpg");
	//f2.extract("surf", "/home/maruya-t/git/bow/67m.jpg");
	
	// test_book_saveload(f1,f2);
	// test_index_saveload(f1,f2);
	//test_words(f1, f2);
	
	cout << "c,r:" << f1.descriptor.cols << "," << f1.descriptor.rows << endl;
	cout << "flags:" << f1.descriptor.flags << endl;
	cout << "depth:" << f1.descriptor.depth() << endl;
	cout << "type:"  << f1.descriptor.type() << endl;
	cout << "CV_8U:" << CV_8U << endl;
	cout << "CV_8S:" << CV_8S << endl;
	cout << "CV_16U:" << CV_16U << endl;
	cout << "CV_16S:" << CV_16S << endl;
	cout << "CV_32S:" << CV_32S << endl;
	cout << "CV_32F:" << CV_32F << endl;
	cout << "CV_64F:" << CV_64F << endl;
	cout << "CV_8UC1:" << CV_8UC1 << endl;
	return 0;
	*/
}

