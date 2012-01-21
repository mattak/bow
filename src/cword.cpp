#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include "feature.h"
#include "book.h"
#include "util.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void usage_exit (const char* message) {
  if (message!=NULL) {
    cerr << message << endl;
  }
  cerr << "usage: <book file> <desc file>+ <option>" << endl;
	cerr << "option:" << endl;
	cerr << "  -l <label> : label for output data" << endl;
	cerr << "  -k <knn>   : knn votes" << endl;
	cerr << "ex:" << endl;
	cerr << " cword t.book t.orb > t.word" << endl;
	cerr << " cword t.book frame1.orb frame2.orb -knn 3 > t.word" << endl;
	cerr << " cword t.book frame0.orb frame1.orb frame2.orb > t.word" << endl;
	cerr << " cword t.book t.orb -l 0 > t.word " << endl;
	exit(0);
}

inline
void output (Book& book, vector< Ptr<Feature> >& desc, int knn, const char *label, ostream& os){
	Mat word;
	book.getmultiframeword(desc, word, knn);
	put_as_libsvm(word, os, label);
}

int main (int argc, char **argv) {
	if (argc<3) {
		usage_exit("argument size is not enough");
	}
	
	const char *bookfile = NULL;
	const char *descfile = NULL;
	const char *label    = NULL;
	int knn = 1;

	int result;
	while ((result=getopt(argc,argv,"l:k:"))!=-1) {
		switch (result) {
		case 'l':
			label = optarg;
			break;
		case 'k':
			knn = atoi(optarg);
			break;
		case ':':
		case '?':
			usage_exit("unknown option");
			break;
		}
	}
	
	bookfile = (optind<argc) ? argv[optind] : NULL;
	//descfile = (optind+1<argc) ? argv[optind+1] : NULL;

	Book book;
	book.load(bookfile);

	if (optind+1>=argc) {
		usage_exit("no descriptor file");
	}

	// load
	vector< Ptr<Feature> > desc;
	for (int i=optind+1;i<argc;i++) {
		vector< Ptr<Feature> > d;
		d = FeatureFactory::loadFeatures(argv[i]);
		for(int j=0;j<d.size();j++) {
			desc.push_back(d.at(j));
		}
	}

	cerr << desc.size() <<  " frames readed" << endl;

//	if (savefile != NULL) {
//		ofstream ofs;
//		ofs.open(savefile);
//		output(book, desc, knn, label, ofs);
//		ofs.close();
//	}
	
	output(book, desc, knn, label, cout);	
	return 0;
}

