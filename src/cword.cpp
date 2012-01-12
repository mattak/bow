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
  cerr << "usage: <book file> (<desc file> <word file>?)? <option>" << endl;
	cerr << "option:" << endl;
	cerr << "  -l <label> : label for output data" << endl;
	cerr << "  -k <knn>   : knn votes" << endl;
	cerr << "ex:" << endl;
	cerr << " cword t.book < t.orb > t.word" << endl;
	cerr << " cword t.book t.orb > t.word" << endl;
	cerr << " cword t.book t.orb t.word" << endl;
	cerr << " cword t.book t.orb t.word -l 0" << endl;
	exit(0);
}

inline
void output (Book& book, vector< Ptr<Feature> >& desc, int knn, const char *label, ostream& os){
	Mat word;
	book.getwords(desc, word, knn);
	put_as_libsvm(word, os, label);
}

int main (int argc, char **argv) {
	if (argc<2) {
		usage_exit("argument size is not enough");
	}
	
	const char *bookfile = argv[1];
	const char *descfile = NULL;
	const char *savefile = NULL;
	const char *label    = NULL;
	int knn = 3;

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

	descfile = (optind+1<argc) ? argv[optind+1] : NULL;
	savefile = (optind+2<argc) ? argv[optind+2] : NULL;

	Book book;
	book.load(bookfile);

	// load
	vector< Ptr<Feature> > desc;
	if (descfile != NULL) {
		desc = FeatureFactory::loadFeatures(descfile);
	}
	else {
		desc = FeatureFactory::loadFeatures(cin);
	}

	if (savefile != NULL) {
		ofstream ofs;
		ofs.open(savefile);
		output(book, desc, knn, label, ofs);
		ofs.close();
	}
	else {
		output(book, desc, knn, label, cout);	
	}
	return 0;
}

