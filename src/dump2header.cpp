#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include <fstream>

#include "feature.h"
#include "book.h"
#include "util.h"

using namespace std;
using namespace cv;

void usage_exit () {
	cerr << "usage : dump2header <bookfile> <type>" << endl;
	exit(1);
}

int main (int argc, char **argv) {
	if (argc < 3) {
		usage_exit();
	}
	
	const char * inputfile = argv[1];
	const char * type      = argv[2];
	Book book;
	book.load(inputfile);

	cout << "const int book_" << type << "_rows = " << book.book.rows << ";" << endl;
	cout << "const int book_" << type << "_cols = " << book.book.cols << ";" << endl;
	cout << "float book_" << type << "[" << (book.book.rows * book.book.cols) << "] = " << format(book.book,"C") << ";" << endl;

	return 0;
}


