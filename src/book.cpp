#include <cv.h>
#include "book.h"
#include "util.h"
#include <stdio.h>

#define rep(i,n) for(int i=0;i<n;i++)

using namespace cv;
using namespace std;

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
    throw "balanced kmeans is failed, datasize is less than k";
    exit(0);
  }
  Mat newcluster = Mat::zeros(k,cluster.cols,cluster.type());
  rep(i,cluster.rows) {
    rep(j,cluster.cols) {
      newcluster.at<int>(i,j) = cluster.at<int>(i,j);
    }
  }
  while (ncluster<k) {
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

  // clustering
  if (points.rows<dstk) {
    k=1;
    cluster = Mat::zeros(1,points.cols,points.type());
    label = Mat::zeros(points.rows,1,CV_32SC1);
    rep(i,points.rows) {
      rep(j,points.cols) {
        cluster.at<float>(0,j) += points.at<float>(i,j);
      }
    }
    rep(j,points.cols) {
      cluster.at<float>(0,j) = cluster.at<float>(0,j) / points.rows;
    }
    return 1;
  }

  kmeans(
    points,
    k,
    label,
    cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER,10,1.0),
    1,
    KMEANS_PP_CENTERS,
    cluster
  );
  
  if (level==1 || k==1) {
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
  rep(i,k) {
    kpoints[i]=Mat::zeros(nums[i],points.cols,points.type());
  }
  
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
  int label_nums[k];
  rep(c,k) {
    Mat tmplabel;
    label_nums[c] = hierarchical_kmeans(k, kpoints[c], tmplabel, kclusters[c], level-1);
    sumk += label_nums[c];
    klabels[c] = tmplabel;
  }

  int total_label[k];
  rep(i,k) {
    total_label[i] = (i<=0) ? 0 : (total_label[i-1]+label_nums[i-1]);
  }
  
  // set result
  cluster = Mat::zeros(sumk, points.cols, points.type());
  label = Mat::zeros(points.rows, 1, CV_32SC1);
  points = Mat::zeros(points.rows, points.cols, points.type());
  int p[3] = {0,0,0};
  rep(i,k) {
    rep(y,kclusters[i].rows) {
      rep(x,kclusters[i].cols) {
        cluster.at<float>(p[0],x) = kclusters[i].at<float>(y,x);
      }
      p[0]++;
    }
    rep(y,klabels[i].rows) {
      label.at<int>(p[1],0) = klabels[i].at<int>(y,0) + total_label[i];
      p[1]++;
    }
    rep(y,kpoints[i].rows) {
      rep(x,kpoints[i].cols) {
        points.at<float>(p[2],x) = kpoints[i].at<float>(y,x);
      }
      p[2]++;
    }
  }

  // increase cluster.
  if (sumk<dstk) {
    // balanced clustering
    sumk = balanced_kmeans (dstk, points, label, cluster);
  }
  return sumk;
}

int Book::makebook (int k, const int hierarchical_level) {
 	Feature::DataType dt;
	try {
		if (features.size()<1) {
			throw "there is not features in vector.";
		}
		dt = features.at(0)->data_type();
		for (int i=1;i<features.size();i++) {
			if (dt!=features.at(i)->data_type()) {
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
    rows += features.at(i)->descriptor.rows;
    cols = features.at(i)->descriptor.cols;
  }
  
	if (dt==Feature::UCHAR) {
		points = Mat::zeros(rows,cols*8,CV_32FC1);
	}
	else {
		points = Mat::zeros(rows,cols,CV_32FC1);
	}
  
  int sy = 0;
  for (int i=0;i<features.size();i++) {
    Ptr<Feature> f = features.at(i);
		Mat tmp = f->floated_descriptor();
		for (int y=0;y<tmp.rows;y++,sy++) {
			for (int x=0;x<tmp.cols;x++) {
				points.at<float>(sy,x) = tmp.at<float>(y,x);
			}
		}
  }

  Mat label;
	int resk = hierarchical_kmeans(k,points,label,book,hierarchical_level);
	return resk;
}

void Book::getword (Ptr<Feature> &f, Mat& dst_word, flann::Index& idx, const int knn) {
  Mat tmp = f->floated_descriptor();
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

void Book::getwords (vector< Ptr<Feature> >& fs, Mat& dst_words, const int knn) {
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
  vector< Ptr<Feature> > fs;
  fs = FeatureFactory::loadFeatures(file);
	cerr << "feature load success : " << fs.size() << endl;
	for (int i=0;i<fs.size();i++) {
		cerr << fs.at(i)->type << endl;
	}
	getwords(fs, dst_words, knn);
}

void Book::add (const char *file) {
  vector< Ptr<Feature> > tmp = FeatureFactory::loadFeatures(file);
  rep(i,tmp.size()) {
    features.push_back(tmp.at(i));
  }
}

void Book::add (Ptr<Feature> &f) {
  features.push_back(f);
}

ostream& Book::write (ostream& os, const bool bin) {
  os << "^" << endl;
  os << "type book" << endl;
  os << "feature " << features.at(0)->type << endl;
  os << "size " << book.rows << " " << book.cols << endl;
	os << "binary " << bin << endl;
  os << "$" << endl;
  if (bin) {
		for (int y=0; y<book.rows; y++) {
			for (int x=0; x<book.cols; x++) {
				float f = book.at<float>(y,x);
				os.write((char *)&f, sizeof(float));
			}
		}
	}
	else {
		for (int y=0; y<book.rows; y++) {
			os << book.at<float>(y,0);
			for (int x=1; x<book.cols; x++) {
				os << " " << book.at<float>(y,x);
			}
			os << endl;
		}
	}
  return os;
}

bool Book::read (istream& is, HeaderInfo& info) {
  bool done = false;
	bool readbinary = false;
  {
    readbinary = (info.data["binary"].front()=="1") ? true : false;
    int rows = atoi(info.data["size"][0].c_str());
    int cols = atoi(info.data["size"][1].c_str());
    book = Mat::zeros(rows, cols, CV_32FC1);
  }
	string line;
	if (readbinary) {
		for (int y=0;y<book.rows;y++) {
			for (int x=0;x<book.cols;x++) {
				float f;
				is.read((char *)&f, sizeof(float));
				book.at<float>(y,x) = f;
			}
		}
    done = true;
	}
  else {
    int y = 0;
	  while (!done && is && getline(is,line)) {
			vector<string> strs = split(line, " ");
			for (int x=0;x<strs.size();x++) {
				float f = atof(strs.at(x).c_str());
				book.at<float>(y,x) = f;
			}
			y++;
      if (y>=book.rows) done = true;
    }
	}
  return done;
}

