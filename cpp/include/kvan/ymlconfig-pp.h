// -*- c++ -*-
#ifndef __KVAN_YMLCONFIG_PP__HH__
#define __KVAN_YMLCONFIG_PP__HH__

#include <vector>
#include <string>
#include <set>
#include <sstream>
using namespace std;

class YMLConfigPP {
private:
  ostringstream out;
  set<string> include_files;
  
  vector<string> pp_pathes;
  void process_include(const string& include_line);
  
public:
  YMLConfigPP(const vector<string>& pp_pathes);
  pair<string, bool> find_yml_file(const string& yml);
  void run_pp(const string& yml_fn);
  void get_pp_content(string* out_s) { *out_s = out.str(); }
};

#endif
