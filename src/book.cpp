#include <cv.h>
#include "book.h"
#include "util.h"

using namespace cv;
using namespace std;

Book::Book(){
  
}

Book::~Book(){

}

int Book::makebook (int k, bool hierarchical) {
 	Feature::DataType dt;
	try {
		if (features.size()<1) {
			throw "there is not features in vector.";
		}
		dt = features.at(0).data_type();
		for (int i=1;i<features.size();i++) {
			if (dt!=features.at(i).data_type()) {
				throw "feature type is not consistent in feature vector.";
			}
		}
	}
	catch (const char *str) {
		cerr << "error: " << str << endl;
		exit(1);
	}
	
	Mat points;
  int rows=0, cols=0;
  for (int i=0;i<features.size();i++) {
    rows += features.at(i).descriptor.rows;
    cols = features.at(i).descriptor.cols;
  }
  	
	if (dt==Feature::UCHAR) {
		points = Mat::zeros(rows,cols*8,CV_32FC1);
	}
	else {
		points = Mat::zeros(rows,cols,CV_32FC1);
	}
  
  int sy = 0;
  for (int i=0;i<features.size();i++) {
    Feature f = features.at(i);
		Mat tmp = f.floated_descriptor();
		for (int y=0;y<tmp.rows;y++,sy++) {
			for (int x=0;x<tmp.cols;x++) {
				points.at<float>(sy,x) = tmp.at<float>(y,x);
			}
		}
  }

  Mat label;
	int resk = 1;
	if (hierarchical) {}
	else {}
	kmeans(
		points,
		k,
		label,
		cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),
		1,
		KMEANS_PP_CENTERS,
		book
	);
	resk = k;
	return k;
}

void Book::getword (Feature& f, Mat& dst_word, flann::Index& idx, const int knn) {
/*
	if (f.data_type()==Feature::UCHAR) {
		Mat tmp = f.floated_descriptor();
		getword(tmp, dst_word, idx, knn);
	}
	else {
		getword(f.descriptor, dst_word, idx, knn);
	}
*/
  Mat tmp = f.floated_descriptor();
  getword(tmp, dst_word, idx, knn);
}

void Book::getword (Mat& src, Mat& dst_word, flann::Index& idx, const int knn) {
	Mat indices;
	Mat dists;
	flann::SearchParams s;
	idx.knnSearch(src, indices, dists, knn, s);
	dst_word = Mat::zeros(1, book.rows, CV_32FC1);
	Mat vote = Mat::zeros(1, book.rows, CV_32SC1);
	for (int i=0;i<indices.rows;i++) {
		for (int j=0;j<indices.cols;j++) {
			int p = indices.at<int>(i,j);
			vote.at<int>(0,p) += 1;
		}
	}
	
	long long sum = 0;
	for (int i=0;i<vote.cols;i++) {
		sum += vote.at<int>(0,i);
	}
	
	for (int i=0;i<dst_word.cols;i++) {
		dst_word.at<float>(0,i) = (float)(vote.at<int>(0,i))/sum;
	}
}

void Book::getwords (vector<Feature>& fs, Mat& dst_words, const int knn) {
	dst_words = Mat::zeros(fs.size(), book.rows, CV_32FC1);

	flann::KDTreeIndexParams kdp;
	flann::Index idx(book,kdp);

	for (int i=0;i<fs.size();i++) {
		Mat dst;
		getword(fs.at(i), dst, idx, knn);
		for (int x=0; x<dst.cols; x++) {
			dst_words.at<float>(i,x) = dst.at<float>(0,x);
		}
	}
}

void Book::getwords (const char *file, Mat& dst_words, const int knn) {
	vector<Feature> fs;
	ifstream ifs;
	ifs.open(file, ios::in|ios::binary);
	while (ifs) {
		Feature f;
		if (f.read(ifs)) {
			fs.push_back(f);
		}
	}
	ifs.close();
	cerr << "feature load success : " << fs.size() << endl;
	for (int i=0;i<fs.size();i++) {
		cerr << fs.at(i) << endl;
	}
	getwords(fs, dst_words, knn);
}

void Book::add (const char *file) {
  Feature f;
  ifstream ifs;
	ifs.open(file, ios::in|ios::binary);
  while (ifs) {
    Feature f;
    if (f.read(ifs)) {
			features.push_back(f);
		}
  }
	ifs.close();
}

void Book::add (Feature& f) {
  features.push_back(f);
}

/*
void Book::save (const char* bookfile, const char* treefile) {
	cout << "tree string" << endl;
	cout << "try tree saving:" << treefile << endl;
	tree.save(treefile);
	cout << "try save book" << endl;
	save_book(bookfile);
	cout << "end saving" << endl;
}

void Book::load (const char* bookfile, const char* treefile) {
	string treestring(treefile);
	load_book(bookfile);
	tree.load(book,treestring);
}
*/

void Book::save_book (const char* file, const bool bin) {
  ofstream ofs;
	if (bin) {
		ofs.open(file, ios::out | ios::binary);
	}
	else {
		ofs.open(file, ios::out);
	}
  write_book(ofs, bin);
  ofs.close();
}

void Book::load_book (const char* file) {
  ifstream ifs;
	ifs.open(file, ios::in | ios::binary);
  read_book(ifs);
  ifs.close();
}

void Book::write_book (ofstream& ofs, const bool bin) {
  ofs << "^" << endl;
  ofs << "type book" << endl;
  ofs << "feature " << features.at(0).type << endl;
  ofs << "size " << book.rows << " " << book.cols << endl;
	ofs << "binary " << bin << endl;
  ofs << "$" << endl;
  if (bin) {
		for (int y=0; y<book.rows; y++) {
			for (int x=0; x<book.cols; x++) {
				float f = book.at<float>(y,x);
				ofs.write((char *)&f, sizeof(float));
			}
		}
	}
	else {
		for (int y=0; y<book.rows; y++) {
			ofs << book.at<float>(y,0);
			for (int x=1; x<book.cols; x++) {
				ofs << " " << book.at<float>(y,x);
			}
			ofs << endl;
		}
	}
}

void Book::read_book (ifstream& ifs) {
  int y=0, x=0;
  bool header = false;
	bool readbinary = false;
	{
		string line;
		while (ifs && getline(ifs,line)) {
			if (line == "^") {
				header = true;
				continue;
			}
			else if(line == "$") {
				header = false;
				if (readbinary) {
					for (int y=0;y<book.rows;y++) {
						for (int x=0;x<book.cols;x++) {
							float f;
							ifs.read((char *)&f, sizeof(float));
							book.at<float>(y,x) = f;
						}
					}
					break;
				}
				else {
					continue;
				}
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
				else if (tag=="binary") {
					if (strs.at(1)=="1") {
						readbinary = true;
					}
					else {
						readbinary = false;
					}
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
}

