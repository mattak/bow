#include <cv.h>
#include <cxcore.h>
#include "feature.h"
#include <unistd.h>
#include <iostream>

using namespace std;

void usage_exit () {
	cerr << "usage: <image file> <output file> (<options>)" << endl;
	cerr << "  -s show" << endl;
	cerr << "  -t <descriptor type>: surf, sift, orb" << endl;
	cerr << "  -p output descriptor is plain text" << endl;
	cerr << "  -g grid on?" << endl;
	exit(0);
}

int main (int argc, char **argv) {
	if (argc<3) {
		usage_exit();
	}
	
	string type = "surf";
	bool show   = false;
	bool binary = true;
	bool grid   = false;
	int result;
	while ((result=getopt(argc,argv,"st:pg"))!=-1) {
		switch (result) {
		case 's':
			show = true;
			break;
		case 't':
			type = string(optarg);
			break;
		case 'p':
			binary = false;
			break;
		case 'g':
			grid = true;
			break;
		case ':':
		case '?':
			usage_exit();
			break;
		}
	}

	if (!(optind+2>=argc || (optind+1>=argc && show))) {
		usage_exit();
	}
	
	char *infile  = argv[optind];
	char *outfile = argv[optind+1];
	
	KeyPointFeature f;
	if (grid) {
		f.add_grid(12, 12, 6);
	}
	f.extract(type.c_str(),infile);
	
	if (show) {
		f.show();
	}
	else {
		f.save(outfile,binary);
	}
	return 0;
}


