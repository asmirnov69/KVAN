#include <regex>
#include <fstream>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <kvan/ymlconfig-pp.h>
#include <kvan/string-utils.h>

YMLConfigPP::YMLConfigPP(const vector<string>& pp) {
  for (auto& p: pp) {
    pp_pathes.push_back(evaluate_dollar_var_expr(p));
  }
}

pair<string, bool> YMLConfigPP::find_yml_file(const string& yml_fn)
{
  for (auto pp_path: pp_pathes) {
    string yml_pn;
    yml_pn = pp_path;
    yml_pn += "/";
    yml_pn += yml_fn;
    yml_pn = evaluate_dollar_var_expr(yml_pn);
    if (fs::exists(yml_pn)) {
      return make_pair(yml_pn, true);
    }
  }
  return make_pair("", false);
}

void YMLConfigPP::process_include(const string& include_line)
{
  regex re(R"D(!include\s+\<([\w/\.\-]+)\>)D");

  smatch m;
  regex_search(include_line, m, re);
  if (!(m.ready() && m.size() == 2)) {
    throw runtime_error(__func__);
  }

  auto include_file = find_yml_file(m[1]);
  if (include_file.second == false) {
    ostringstream mm; mm << "YMLConfigPP::process_include: can't find " << m[1].str();
    throw runtime_error(mm.str());
  }
  
  if (auto it = this->include_files.find(include_file.first);
      it == this->include_files.end()) {
    this->include_files.insert(include_file.first);
    this->run_pp(include_file.first);
  }
}

void YMLConfigPP::run_pp(const string& yml_fn)
{
  ifstream yml_fn_in(yml_fn);
  if (!yml_fn_in) {
    ostringstream m; m << "YMLConfigPP:: can't read file " << yml_fn;
    throw runtime_error(m.str());
  }

  out << "# file " << yml_fn << endl;
  string l;
  while (getline(yml_fn_in, l)) {
    if (l.find("!include") != string::npos) {
      out << "# " << l << endl;
      process_include(l);
    } else {
      out << l << endl;
    }
  }

  out << "#" << endl;
  out << "# end of file " << yml_fn << endl;
}
