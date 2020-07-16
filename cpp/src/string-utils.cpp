#include <stdexcept>
#include <sstream>
#include <iostream>
#include <regex>
using namespace std;

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <kvan/string-utils.h>
#include <kvan/topdir.h>

vector<string> string_split(const string& s, char del)  
{
  vector<string> ret;
  string p_s;
  istringstream in(s);
  while (getline(in, p_s, del)) {
    ret.push_back(p_s);
  }
  return ret;
}

string string_join(const vector<string>& vs, char del)
{
  ostringstream out;
  for (size_t i = 0; i < vs.size(); i++) {
    out << vs[i];
    if (i + 1 < vs.size()) {
      out << del;
    }
  }
  return out.str();
}

vector<string> string_join(const vector<vector<string>>& vvs, char del)
{
  vector<string> ret;
  for (auto& v: vvs) {
    ret.emplace_back(string_join(v, del));
  }
  return ret;
}

// from https://stackoverflow.com/a/25829233/1181482
inline string& ltrim(string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}
inline string& rtrim(string& s, const char* t = " \t\n\r\f\v")
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

string string_strip(const string& s)
{
  string ret(s);
  return rtrim(ltrim(ret));
}

vector<string> string_strip(const vector<string>& vs)
{
  vector<string> ret;
  for (auto& s: vs) {
    ret.push_back(string_strip(s));
  }
  return ret;
}

// expands $ENV{env-var-name} and ${top-dir}
string evaluate_dollar_var_expr(const string& dv_expr)
{
  string res = dv_expr;

  regex env_re(R"D(\$ENV\{(\w+)\})D");
  string s = dv_expr;
  for (smatch m; regex_search(s, m, env_re); s = m.suffix()) {
    if (m.ready() && m.size() == 2) {
      string env_var = m[1];
      string env_var_value = getenv(env_var.c_str());
      string repl = "\\$ENV\\{" + env_var + "\\}";
      res = regex_replace(res, regex(repl), env_var_value);
    }
  }

  fs::path top_dir = TopDir::get()->get_topdir();
  res = regex_replace(res, regex("\\$\\{top-dir\\}"), top_dir.string());

  fs::path l_etc_dir = top_dir / "etc";
  fs::path upper_etc_dir = fs::absolute(top_dir / ".." / "etc");
  bool l_etc_dir_f = fs::exists(l_etc_dir);
  bool upper_etc_dir_f = fs::exists(upper_etc_dir);
  if (l_etc_dir_f && upper_etc_dir_f) {
    throw runtime_error("both ${top-dir}/etc and ${top-dir}/../etc exists");
  }

  auto etc_dir = l_etc_dir_f ? l_etc_dir : upper_etc_dir;
  res = regex_replace(res, regex("\\$\\{etc-dir\\}"), etc_dir.string());
  return res;
}

string getusername()
{
  string ret(getenv("USER"));
  return ret;
}
