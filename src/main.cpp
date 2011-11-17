#include <iostream>
#include "bow.h"
int main(int argc, char** argv){
	using namespace std;
	Feature f;
	//f.add_grid(20,20,10);
/*
	f.extract("surf",`any jpg`);
	cout << f.descriptor.rows << endl;
	f.save("atmp.dsc",true);
*/	
	f.load("atmp.dsc",true);
	cout << "last :" << f.descriptor.at<float>(f.descriptor.rows-1,f.descriptor.cols-1) << endl;
	cout << "klast:" << f.keypoint.at(f.keypoint.size()-1).pt.x << endl;
	f.show();	
	return 0;
}
