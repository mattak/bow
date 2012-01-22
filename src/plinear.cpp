#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define rep(i,n) for(int i=0;i<n;i++)
#define eq(str1,str2) if(strcmp(str1,str2)==0)
#define nextok() strtok(NULL," :")
#define f_nextok() atof(strtok(NULL," :"))
#define i_nextok() atoi(strtok(NULL," :"))
#define s_nextok() string(strtok(NULL," :"))

using namespace std;

void usage_exit(int exitval) {
	cerr << "usage : <type> <modelfile> <testfile>" << endl;
	cerr << "type  : ll -> liblinear" << endl;
	cerr << "      : ls -> libsvm linear" << endl;
	exit(exitval);
}

bool readline (vector<double>& datas, istream& is, int n, int* correct) {
	int label;
	is >> label;
	*correct = label;
	if (!is) {
		return false;
	}
	rep(i,n) {
		string pair;
		is >> pair;
		string::size_type idx = pair.find(":");
		double val = atof(pair.substr(idx+1).c_str());
		datas.push_back(val);
	}
	return true;
}

inline
double predict_value (double *sv, int n, double bias, double* feat) {
	double dec = 0;
	rep(i,n) {
		dec += sv[i] * feat[i];
	}
	dec -= bias;
	return dec;
}

inline
double sigmoid (double x) {
	return (2.0/(1.0+exp(-x))-1.0);
}

void test_and_put (const char* testfile, double *w, int nr_feature, double bias, int label[2]) {
	vector<int> result_labels;
	vector<int> correct_labels;
	vector<double> predict_values;
	int corrects = 0;
	int totals   = 0;
	ifstream ifs(testfile);
	while(ifs) {
		vector<double> data;
		int readlabel = 0;
		if (!readline(data,ifs,nr_feature, &readlabel)) {
			break;
		}
		correct_labels.push_back(readlabel);

		double decision = predict_value(w,nr_feature,bias,&data[0]);
		decision = sigmoid(decision);
		predict_values.push_back(decision);

		int res = (decision>0) ? label[0] : label[1];
		result_labels.push_back(res);

		if (readlabel==res){
			corrects++;
		}
		totals++;
	}
	ifs.close();

	cout << "# " << corrects << "/" << totals << " " << ((double)corrects/totals) << endl;
	rep(i,correct_labels.size()) {
		cout << correct_labels[i] << " ";
		cout << result_labels[i]  << " ";
		cout << predict_values[i] << endl;
	}
}

void calc_liblinear(const char *modelfile, const char *testfile) {
	// read
	// -------------------
	// solver_type <L2R_L2LOSS_SVC_DUAL | L2R_L2LOSS_SVC | L2R_L1LOSS_SVC_DUAL> 
	// nr_class 2
	// label 1 -1
	// nr_feature 1000
	// bias -1
	// w
	// .... number of nr_feature
	ifstream fin(modelfile);
	string line;
	string solver_type;
	int nr_class;
	int label[2];
	int nr_feature;
	int bias;
	vector<double> w;
	fin >> line >> solver_type;
	fin >> line >> nr_class;
	fin >> line >> label[0] >> label[1];
	fin >> line >> nr_feature;
	fin >> line >> bias;
	fin >> line; // w
	cerr << "solver_type " << solver_type << endl;
	cerr << "nr_class " << nr_class << endl;
	cerr << "label " << label[0] << " " << label[1] << endl;
	cerr << "nr_feature " << nr_feature << endl;
	cerr << "bias " << bias << endl;

	rep(i,nr_feature) {
		double v;
		fin >> v;
		w.push_back(v);
	}
	fin.close();

	// test
	//--------------------
	test_and_put(testfile,&w[0],nr_feature,0,label);
}

void calc_libsvm (const char *modelfile, const char* testfile) {
	// svm_type c_svc
	// kernel_type linear
	// nr_class 2
	// total_sv 3548
	// rho 0.59282
	// // probA -0.42213
	// // probB 0.0147693
	// label 1 -1
	// nr_sv 1774 1774
	// SV
	// weight idx:val ...
	// ...
	string line;
	string svm_type;
	string kernel_type;
	int nr_class;
	int total_sv;
	double rho;
	int label[2];
	double probA,probB;
	int nr_sv[2];
	int nr_feature = -1; // void
	vector< vector<double> > sv;
	vector<double> sv_w;
	
	ifstream fin(modelfile);
	while (getline(fin,line)) {
		char *buf = (char*)malloc(sizeof(char)*(line.size()+1));
		strcpy(buf,line.c_str());
	 	char *token = strtok(buf," ");

		eq(token,"svm_type") {
			svm_type = s_nextok();
		}
		else eq(token,"kernel_type") {
			kernel_type = s_nextok();
		}
		else eq(token,"nr_class") {
			nr_class = i_nextok();
		}
		else eq(token,"total_sv") {
			total_sv = i_nextok();
		}
		else eq(token,"rho") {
			rho = f_nextok();
		}
		else eq(token,"label") {
			label[0] = i_nextok();
			label[1] = i_nextok();
		}
		else eq(token,"nr_sv") {
			nr_sv[0] = i_nextok();
			nr_sv[1] = i_nextok();
		}
		else eq(token,"probA") {
			probA = f_nextok();
		}
		else eq(token,"probB") {
			probB = f_nextok();
		}
		else eq(token,"SV") {
			break;
		}
		free(buf);
	}
	//cerr << "svm_type " << svm_type << endl;
	//cerr << "kernel_type " << kernel_type << endl;
	//cerr << "nr_class " << nr_class << endl;
	//cerr << "total_sv " << total_sv << endl;
	//cerr << "rho " << rho << endl;
	//cerr << "label " << label[0] << " " << label[1] << endl;
	//cerr << "nr_sv " << nr_sv[0] << " " << nr_sv[1] << endl;

	while (getline(fin,line)) {
		char *buf = (char*)malloc(sizeof(char)*(line.size()+1));
		char *token;
		double w;
		vector<double> sv_colum;
		int features=0;

		strcpy(buf, line.c_str());
		token = strtok(buf," :");
		w = atof(token);
		sv_w.push_back(w);

		while ((token=nextok())!=NULL) {
			sv_colum.push_back(f_nextok());
			features++;
		}

		sv.push_back(sv_colum);
		nr_feature = (nr_feature<0) ? features : nr_feature;
		if (nr_feature!=features) {
			cerr << "input vector is not sequencial and not consistent!!" << endl;
			exit(1);
		}
		free(buf);
	}
	fin.close();

	double* computed = (double*)malloc(sizeof(double)*nr_feature);
	rep(i,nr_feature) {
		computed[i]=0;
	}

	rep(i,sv.size()) {
		rep(j,nr_feature){
			computed[j] += sv_w[i] * sv[i][j];
		}
	}

	// test
	// -----------
	test_and_put(testfile, computed, nr_feature, rho, label);
}

// liblinear modelfile calculate sample
int main (int argc, char **argv) {
	const char *type      = (argc>1) ? argv[1] : NULL;
	const char *modelfile = (argc>2) ? argv[2] : NULL;
	const char *testfile  = (argc>3) ? argv[3] : NULL;
	if (modelfile==NULL || testfile==NULL) {
		usage_exit(0);
	}
	
	eq(type,"ll") {
		calc_liblinear(modelfile, testfile);
	}
	else eq(type,"ls") {
		calc_libsvm(modelfile, testfile);
	}
	else {
		cerr << "unknown options" << endl;
	}
	return 0;
}

