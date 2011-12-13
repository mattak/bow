#include <cv.h>
#include "book.h"
#include "util.h"
#include <stdio.h>

#define rep(i,n) for(int i=0;i<n;i++)

using namespace cv;
using namespace std;

Book::Book(){
}

Book::~Book(){
}

// before use this function, please set initial label and cluster.
int balanced_kmeans (int k, Mat& points, Mat& label, Mat& cluster) {
  int ncluster = 0;
  rep(i,label.rows) {
    if (ncluster < label.at<int>(i,0)) {
      ncluster = label.at<int>(i,0);
    }
  }
  ncluster++;
  if (points.rows<k) {
    cerr << "balanced kmeans is failed, datasize is less than k" << endl;
    exit(0);
  }
  cout << "before new cluster" << endl;
  Mat newcluster = Mat::zeros(k,cluster.cols,cluster.type());
  rep(i,cluster.rows) {
    rep(j,cluster.cols) {
      newcluster.at<int>(i,j) = cluster.at<int>(i,j);
    }
  }
  cout << "k,ncluster:" << k << "," << ncluster << endl;
  while (ncluster<k) {
    cout << "ncluster:" << ncluster << endl;
    // find max data size of cluster
    int max_data_size = 0;
    int numlabel[ncluster];
    rep(i,ncluster) {
      numlabel[i]=0;
    }
    rep(i,label.rows) {
      int p = label.at<int>(i,0);
      numlabel[p]++;
    }
    int max_numlabel=0;
    int max_label=0;
    rep(i,ncluster) {
      if (max_numlabel < numlabel[i]) {
        max_numlabel = numlabel[i];
        max_label = i;
      }
      cout << "numlabel[" << i << "]:" << numlabel[i] << endl;
    }
    // clustering
    Mat tmp_label;
    Mat tmp_points = Mat::zeros(max_numlabel,points.cols,points.type());
    Mat tmp_cluster;
    int p=0;
    int src_pos[max_numlabel];
    rep(i,points.rows) {
      if (label.at<int>(i,0)==max_label) {
        rep(j,points.cols) {
          tmp_points.at<float>(p,j) = points.at<float>(i,j);
        }
        src_pos[p] = i;
        p++;
      }
    }
    kmeans(
      tmp_points,
      2,
      tmp_label,
      cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),
      1,
      KMEANS_PP_CENTERS,
      tmp_cluster
    );
    
    // reset label, newcluster
    rep(i,max_numlabel) {
      if (tmp_label.at<int>(i,0)==1) {
        label.at<int>(src_pos[i],0) = ncluster;
      }
    }
    rep(j,tmp_cluster.cols) {
      newcluster.at<float>(max_label,j) = tmp_cluster.at<float>(0,j);
      newcluster.at<float>(ncluster,j) = tmp_cluster.at<float>(1,j);
    }
    cout << newcluster << endl;
    ncluster++;
  }
  // copy cluster
  cluster = Mat::zeros(k,newcluster.cols,newcluster.type());
  rep(i,newcluster.rows) {
    rep(j,newcluster.cols) {
      cluster.at<float>(i,j) = newcluster.at<float>(i,j);
    }
  }
  return ncluster;
}

int hierarchical_kmeans (int k, Mat& points, Mat& label, Mat& cluster, int level) {
  int dstk = pow(k,level);
  if (level<0) {
    cerr << "level failed" << endl;
  }
  if (points.rows<dstk) {
    cerr << "points size is less than k" << endl;
    label = Mat::zeros(points.rows, 1, CV_32SC1);
    cluster = Mat::zeros(1, points.cols, CV_32FC1);
    // here will writer new cluster for one 
    return 1;
  }
  // clustering
  cout << "before clustering[" << level << "]:" << k << endl;
  cout << points << endl;
  Mat tmp_label;
  kmeans(
    points,
    k,
    tmp_label,
    cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),
    1,
    KMEANS_PP_CENTERS,
    cluster
  );
  cout << "label copy:" << tmp_label.type() << "," << tmp_label.rows << "," << tmp_label.cols << endl;
  cout << tmp_label << endl;
  //label.release();
  label = Mat::zeros(tmp_label.rows,tmp_label.cols,CV_32SC1);
  cout << "craeted label" << endl;
  rep(i,label.rows) {
    rep(j,label.cols) {
      label.at<int>(i,j) = tmp_label.at<int>(i,j);
    }
  }
  cout << "after clustering[" << level << "]:" << k << endl;
  if (level==1) {
    return k;
  }
  
  // divide 
  int nums[k];
  int pos[k];
  Mat kpoints[k];
  rep(i,k) {
    nums[i] = 0;
    pos[i] = 0;
  }
  rep(i,label.rows) {
    nums[label.at<int>(i,0)]++;
  }
  rep(i,k)
    kpoints[i]=Mat::zeros(nums[i],points.cols,points.type());
  rep(i,k)
    cout << "level:" << level << ",nums[" << i << "]:" << nums[i] << endl;
  
  rep(i,points.rows) {
    int p = label.at<int>(i,0);
    rep(j,points.cols) {
      kpoints[p].at<float>(pos[p],j) = points.at<float>(i,j);
    }
    pos[p]++;
  }
  
  // reclustering
  int sumk = 0;
  Mat kclusters[k];
  Mat klabels[k];
  rep(i,k) {
    sumk += hierarchical_kmeans(k, kpoints[i], klabels[k], kclusters[i], level-1);
  }
  cout << "reclustering true:" << level << "," << sumk << endl;
  // set result
  cluster = Mat::zeros(sumk, points.cols, points.type());
  label = Mat::zeros(sumk, 1, CV_32SC1);
  int p=0;
  rep(i,k) {
    rep(y,kclusters[i].rows) {
      rep(x,kclusters[i].cols) {
        cluster.at<float>(p,x) = kclusters[i].at<float>(y,x);
      }
      label.at<int>(p,0) = klabels[i].at<int>(y,0);
      p++;
    }
  }
  // increase cluster.
  if (sumk<dstk) {
    // balanced clustering
    balanced_kmeans (dstk, points, label, cluster);
  }
  return cluster.rows;
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
	if (hierarchical) {
    int level = 0;
    int tmpk = k;
    while (tmpk>0) {
      level++;
      tmpk=tmpk>>1;
    }
    resk = hierarchical_kmeans(tmpk,points,label,book,level);
  }
	else {
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

