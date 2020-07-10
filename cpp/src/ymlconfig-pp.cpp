#include <regex>
#include <fstream>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <kvan/ymlconfig-pp.h>

YMLConfigPP::YMLConfigPP(const vector<string>& pp) {
  pp_pathes = pp;
}

static pair<string, bool>
find_yml_file(const string& yml_fn, const vector<string>& pp_pathes)
{
  for (auto pp_path: pp_pathes) {
    string yml_pn;
    yml_pn = pp_path;
    yml_pn += "/";
    yml_pn += yml_fn;
    if (fs::exists(yml_pn)) {
      return make_pair(yml_pn, true);
    }
  }
  return make_pair("", false);
}

void YMLConfigPP::process_include(const string& include_line)
{
  regex re(R"D(!include\s+([\w/\.]+))D");

  smatch m;
  regex_search(include_line, m, re);
  if (!(m.ready() && m.size() == 2)) {
    throw runtime_error(__func__);
  }

  string include_file = m[1];
  if (auto it = this->include_files.find(include_file);
      it == this->include_files.end()) {
    this->include_files.insert(include_file);
    this->run_pp(include_file);
  }
}

void YMLConfigPP::run_pp(const string& yml_fn)
{
  auto yml_file = find_yml_file(yml_fn, pp_pathes);
  if (!yml_file.second) {
    throw runtime_error(__func__);
  }
  
  ifstream yml_fn_in(yml_file.first);
  string l;
  while (getline(yml_fn_in, l)) {
    if (l.find("!include") != string::npos) {
      process_include(l);
    } else {
      out << l << endl;
    }
  }
}
