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
  cerr << "usage: <book file> <save file> (<type>? <desc file>)+" << endl;
	cerr << "ex: t.book t.save -1 true1.orb true2.orb -0 false1.orb ..." << endl;
  exit(0);
}

int main (int argc, char **argv) {
	if (argc<4) {
		usage_exit("argument size is not enough");
	}
	
	const char *bookfile = argv[1];
	const char *savefile = argv[2];
	Book book;
	book.load_book(bookfile);
	ofstream ofs;
	ofs.open(savefile);

	int classid = 0;
	for (int i=3;i<argc;i++) {
		if (argv[i][0] == '-') {
			char *num = &argv[i][1];
			classid = atoi(num);
			continue;
		}
		cerr << "getwords:" << argv[i] << endl;
		Mat word;
		book.getwords(argv[i], word);
		cerr << "put_as_libsvm" << endl;
		put_as_libsvm(classid, word, ofs);
	}
	
	ofs.close();
	return 0;
}

