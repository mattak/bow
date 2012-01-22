#include <cv.h>
#include <cxcore.h>
#include "feature.h"
#include "util.h"
#include <unistd.h>
#include <iostream>
#include <fstream>

#define rep(i,n) for(int i=0;i<n;i++)

using namespace std;

void usage_exit () {
	cerr << "usage: <image file>+ (<options>)" << endl;
	cerr << "  -s : show" << endl;
	cerr << "  -t <descriptor type>: (g)surf, (g)sift, (g)orb, (g)rgb, (g)lab, (g)luv (g)hsv, (g)ycrcb" << endl;
	cerr << "  -p : output descriptor is plain text" << endl;
	cerr << "  -g <x,y,scale>: set grid. preffered 12,12,6 in orb" << endl;
  cerr << "  -i : input descriptor from stdio and show" << endl;
  cerr << "  -o <output file>: output descriptor to file." << endl;
	exit(0);
}

int main (int argc, char **argv) {
  if (argc<=1) {
    usage_exit();
  }
	string type = "surf";
	bool show   = false;
	bool binary = true;
	bool grid   = false;
  bool stdinput = false;
  bool stdoutput = true;
  const char *outputfile;
  vector<int> xgrid;
  vector<int> ygrid;
  vector<int> sgrid;
	int result;
	while ((result=getopt(argc,argv,"st:pg:io:"))!=-1) {
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
		case 'g':{
			grid = true;
      vector<string> xys = split(string(optarg), ",");
      int x = atoi(xys[0].c_str());
      int y = atoi(xys[1].c_str());
      int s = atoi(xys[2].c_str());
      xgrid.push_back(x);
      ygrid.push_back(y);
      sgrid.push_back(s);
			break;
    }
    case 'i':
      stdinput = true;
      break;
    case 'o':
      stdoutput = false;
      outputfile = optarg;
      break;
		case ':':
		case '?':
			usage_exit();
			break;
		}
	}

	if (!stdinput && optind>=argc) {
		usage_exit();
	}

  vector< Ptr<Feature> > features;
  if (stdinput) {
    features = FeatureFactory::loadFeatures(cin);
    rep(i,features.size()) {
      features[i]->show();
    }
    return 0;
  }
  
  const int TYPE_BLOB = 0;
  const int TYPE_COLOR = 1;
  int ftype;
  if (type=="orb" || type=="gorb" ||
      type=="surf" || type=="gsurf" ||
      type=="sift" || type=="gsift") {
    ftype = TYPE_BLOB;
  }
  else if (type=="rgb" || type=="grgb"
				|| type=="lab" || type=="glab"
				|| type=="luv" || type=="gluv"
        || type=="hsv" || type=="ghsv"
			 	|| type=="ycrcb" || type=="gycrcb") {
    ftype = TYPE_COLOR;
  }
  else {
    cerr << "unknown descriptor type" << endl;
    usage_exit();
  }
  
  for (int i=optind;i<argc;i++) {
    Ptr<Feature> f;
    if (ftype==TYPE_BLOB) {
      f = new KeyPointFeature();
    }
    else if (ftype==TYPE_COLOR) {
      f = new ColorPatchFeature();
    }
    if (grid) {
      rep(k,xgrid.size()) {
        f->add_grid(xgrid[k],ygrid[k],sgrid[k]);
      }
    }
    f->extract(type.c_str(),argv[i]);
    features.push_back(f);
  }

	if (show) {
    rep(i,features.size()) {
		  features[i]->show();
    }
	}
	else if(stdoutput) {
		rep(i,features.size()) {
      features[i]->set_io_binary(binary);
      cout << (*features[i]);
    }
	}
  else {
    ofstream ofs;
    ofs.open(outputfile, ios::out | ios::binary);
    rep(i,features.size()) {
      features[i]->set_io_binary(binary);
      ofs << (*features[i]);
    }
    ofs.close();
  }
	return 0;
}


