#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include <fstream>
#include "feature.h"
#include "book.h"
#include "util.h"

#include <time.h>
#include <sys/time.h>

#define die(str) cout<<(str)<<endl;exit(1)

using namespace std;
using namespace cv;

double gettime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

// Feature test
void test_keypointfeature_stdout (int argc, char **argv) {
  if (argc<=1) {
    die("test-keypointfeature-stdout : <anyimg>");
  }
  for (int i=1;i<argc;i++) {
    Ptr<Feature> f = new KeyPointFeature();
    f->extract("orb",argv[i]);
    f->set_io_binary(true);
    cout << *f << endl;
  }
}

void test_keypointfeature_stdin (int argc, char **argv) {
  Ptr<Feature> f = new KeyPointFeature();
  while (cin) {
    cout << "reading..." << endl;
    cin >> *f;
    f->show();
  }
}

void test_colorpatchfeature_stdout (int argc, char **argv) {
  if (argc<=1) {
    die("test-colorpatchfeature-stdout : <anyimg>");
  }
  for (int i=1;i<argc;i++) {
    Ptr<Feature> f = new ColorPatchFeature();
    f->add_grid(5,5,5);
    f->extract("color",argv[i]);
    f->set_io_binary(false);
    cout << *f << endl;
    f->show();
  }
}

void test_colorpatchfeature_stdin (int argc, char** argv) {
  Ptr<Feature> f = new ColorPatchFeature();
  while (cin) {
    cout << "reading ..." << endl;
    cin >> *f;
    f->show();  
  }
}

void test_featurefactory_stdin (int argc, char **argv) {
  vector< Ptr<Feature> > features;
  features = FeatureFactory::loadFeatures(cin);
  cout << "nums:" << features.size() << endl;
  for (int i=0;i<features.size(); i++) {
    features.at(i)->show();
  }
}

void test_featurefactory_filein (int argc, char **argv) {
  if (argc <= 1) {
    die("test-featurefactory-filein : <file>+");
  }
  for (int i=1;i<argc;i++) {
    vector< Ptr<Feature> > features = FeatureFactory::loadFeatures(argv[i]);
    for (int j=0;j<features.size();j++) {
      features[j]->show();
    }
  }
}

/*
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
  double start,end;
  start = gettime();
	flann::Index sidx(book.book, kdp);
	sidx.save("tmp_test.idx");
	end = gettime();
  cout << "knn index creation time is " << (end - start) << endl;
  
	// load
	flann::Index lidx;//(book.book,kdp);
  start = gettime();
	lidx.load(book.book, "tmp_test.idx");
	end = gettime();
  cout << "knn index load time is " << (end - start) << endl;
  
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
*/
void test_hierarchical_kmeans() {
  int rows = 1024;
  int cols = 2;
  srand(2);
  Mat testpoints = Mat::zeros(rows,cols,CV_32FC1);
  for (int i=0; i<rows; i++) {
    for (int j=0; j<cols;j++) {
      testpoints.at<float>(i,j) = (float)rand()/RAND_MAX;
    }
  }
  Mat cluster;
  Mat label;
  int res = hierarchical_kmeans(2, testpoints, label, cluster, 5);

  cout << testpoints << endl;
  cout << cluster << endl;
  cout << res << endl;
  cout << cluster.rows << endl;
}

void save_index_file (Book& book, const char* savefile) {
  flann::KDTreeIndexParams kdparams;
  flann::Index idx(book.book, kdparams);
  idx.save(savefile);
}

// feature -> book
// book -> words
// 
int main (int argc, char** argv) {
	//test_hierarchical_kmeans();
  
  // keypoint
  //test_keypointfeature_stdout(argc,argv);
  //test_keypointfeature_stdin(argc,argv);
  test_colorpatchfeature_stdout(argc,argv);

  // factory
  //test_featurefactory_stdin(argc,argv);
  //test_featurefactory_filein(argc,argv);

  return 0;
}

