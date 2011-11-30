#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include "feature.h"
#include "book.h"
#include <unistd.h>
#include <stdlib.h>

using namespace std;

void usage_exit (const char* message) {
	if (message!=NULL) {
		cerr << message << endl;
	}
  cerr << "usage: (options) <k> <output file> <descs>+" << endl;
	cerr << "  -p : save plain" << endl;
	cerr << "  -h : hierarchical" << endl;
  exit(0);
}

int main (int argc, char **argv) {
	if (argc < 3) {
		usage_exit(NULL);
	}

	bool hierarchical = false;
	bool binary = true;
	
	int result;
	while ((result=getopt(argc,argv,"hp"))!=-1) {
		switch (result) {
		case 'h':
			hierarchical = true;
			break;
		case 'p':
			binary = false;
			break;
		case ':':
		case '?':
			usage_exit("unknown option");
			break;
		}
	}
	
	if (optind+3<argc) {
		usage_exit("argument size is not enough");
	}
	
	int k = atoi(argv[optind]);
	string outfile = string(argv[optind+1]);
	
	Book book;
	for (int i=optind+2;i<argc;i++) {
		cout << "read book:" << i << "," << argv[i] << endl;
		book.add(argv[i]);
	}
	
	cout << "make book" << endl;
	book.makebook(k, hierarchical);
	cout << "save book" << endl;
	book.save_book(outfile.c_str(), binary);
	return 0;
}


