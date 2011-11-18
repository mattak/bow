
#include <vector>
#include <string>
#include "util.h"

using namespace std;
// private
vector<string> split(string str, string delim) {
  vector<string> result;
  int cutAt;
  while ((cutAt = str.find_first_of(delim))!=str.npos) {
    if (cutAt>0) {
      result.push_back(str.substr(0,cutAt));
    }
    str = str.substr(cutAt+1);
  }
  if (str.length()>0) {
    result.push_back(str);
  }
  return result;
}

