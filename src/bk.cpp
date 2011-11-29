#include <cv.h>
#include <cxcore.h>
#include <iostream>
#include "feature.h"
#include "book.h"
#include <unistd.h>

using namespace std;

void usage_exit () {
  cerr << "usage: <k> <descs> (<options>)" << endl;
  cerr << "  -s show" << endl;
  cerr << "  -t <descriptor type>: surf, sift, orb" << endl;
  cerr << "  -p output descriptor is plain text" << endl;
  cerr << "  -g grid on?" << endl;
  exit(0);
}

int main (int argc, char **argv) {
	if (argc < 3) {
		usage_exit();
	}
	return 0;
}


