#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include "feature.h"
#include "book.h"
#include <unistd.h>
#include <stdlib.h>

#define rep(i,n) for(int i=0;i<n;i++)

using namespace std;

void usage_exit (const char* message) {
	if (message!=NULL) {
		cerr << message << endl;
	}
  cerr << "usage: (options) <k> <descs>*" << endl;
	cerr << "  -p : save plain" << endl;
	cerr << "  -h <hierarchical_level>: hierarchical level. k = k^level" << endl;
  cerr << "  -o <output file> : result to save file" << endl;
  cerr << "  -i : input from stdin" << endl;
  exit(0);
}

int main (int argc, char **argv) {
	if (argc < 3) {
		usage_exit(NULL);
	}

	bool binary = true;
  bool stdinput = false;
  bool stdoutput = true;
	const char *outputfile;
  int hierarchical_level = 1;
  
	int result;
	while ((result=getopt(argc,argv,"h:pio:"))!=-1) {
		switch (result) {
		case 'h':
      hierarchical_level = atoi(optarg);
			break;
		case 'p':
			binary = false;
			break;
    case 'i':
      stdinput = true;
      break;
    case 'o':
      stdoutput = false;
      outputfile = optarg;
      break;
		case ':':
		case '?':
			usage_exit("unknown option");
			break;
		}
	}
	
	if (optind+2>=argc || (!stdinput && optind+1>=argc)) {
		usage_exit("argument size is not enough");
	}
	
	int k = atoi(argv[optind++]);
	
	Book book;
	for (int i=optind;i<argc;i++) {
		cerr << "read book:" << i << "," << argv[i] << endl;
		book.add(argv[i]);
	}
  if (stdinput) {
    vector< Ptr<Feature> > features = FeatureFactory::loadFeatures(cin);
    rep(i,features.size()) {
      book.add(features[i]);
    }
  }
	
	cerr << "make book" << endl;
	book.makebook(k, hierarchical_level);
  if (stdoutput) {
    cout << book;
  }
  else {
	  book.save_book(outputfile, binary);
  }
	return 0;
}


