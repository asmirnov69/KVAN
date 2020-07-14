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

vector<string> parse_csv_line(const string& line)
{
  enum class parser_state {
    in_cell, in_cell_expect_second_quote,
    out_of_cell
  };

  vector<string> ret;
  parser_state state{parser_state::out_of_cell};

  string cell;
  for (auto c: line) {
    //cout << "c: " << c << endl;
    if (c == ',') {
      switch (state) {
      case parser_state::in_cell:
	ret.push_back(cell); cell = "";
	state = parser_state::out_of_cell;
	break;
      case parser_state::in_cell_expect_second_quote:
	ret.push_back(cell); cell = "";
	state = parser_state::out_of_cell;
	break;      
      case parser_state::out_of_cell:
	ret.push_back(""); cell = "";
	state = parser_state::out_of_cell;
	break;
      }
    } else if (c == '"') {
      switch (state) {
      case parser_state::in_cell:
	state = parser_state::in_cell_expect_second_quote;
	break;
      case parser_state::in_cell_expect_second_quote: 
	cell += '"';
	state = parser_state::in_cell;
	break;
      case parser_state::out_of_cell:
	state = parser_state::in_cell;
	break;
      }
    } else {
      switch (state) {
      case parser_state::in_cell:
	cell += c;
	state = parser_state::in_cell;
	break;
      case parser_state::in_cell_expect_second_quote:
	{
	  ostringstream m;
	  m << "malformed csv line: " << line
	    << ", cell: " << cell
	    << ", c: " << c;
	  throw runtime_error(m.str());
	}
	break;
      case parser_state::out_of_cell:
	cell += c;
	state = parser_state::in_cell;
	break;	
      }
    }
  }
  
  ret.push_back(cell);
  
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
  fs::path upper_etc_dir = top_dir.parent_path() / "etc";
  int f = int(fs::exists(l_etc_dir));
  f = f + int(fs::exists(upper_etc_dir));
  if (f > 1) {
    throw runtime_error("both ${top-dir}/etc and ${top-dir}/../etc exists");
  }

  auto etc_dir = fs::exists(l_etc_dir) ? l_etc_dir : upper_etc_dir;
  res = regex_replace(res, regex("\\$\\{etc-dir\\}"), etc_dir.string());
  return res;
}

string getusername()
{
  string ret(getenv("USER"));
  return ret;
}
