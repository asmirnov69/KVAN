#include <string>
#include <iostream>
#include <sstream>
#include <regex>
using namespace std;

#include <kvan/string-utils.h>
#include <kvan/fuargs.h>

map<string, pair<vector<string>, string>> Fuargs::action_protos;
map<string, Fuargs::action_func_t> Fuargs::actions;
int Fuargs::argc{0};
char** Fuargs::argv{nullptr};

bool
Fuargs::args::is_compatible_args_proto_list(const vector<string>& args_proto_list) const
{
  bool res = true;
  for (auto [k, v]: arg_map) {
    if (auto it = find(args_proto_list.begin(), args_proto_list.end(), k);
	it == args_proto_list.end()) {
      res = false;
      break;
    }
  }
  return res;
}

string Fuargs::args::get(const char* argname) const
{
  auto it = arg_map.find(argname);
  if (it == arg_map.end()) {
    ostringstream m; m << "no such arg: " << argname;
    throw runtime_error(m.str());
  }

  string arg_value = (*it).second;
  if (arg_value.size() >= 2
      && arg_value[0] == '[' && arg_value.back() == ']') {
    return arg_value.substr(1, arg_value.size() - 2);
  }
  return string_strip(arg_value);
}

void Fuargs::args::dump() const
{
  cerr << "args dump:" << endl;
  for (auto& [k, v]: arg_map) {
    cerr << k << ": " << v << endl;
  }
  cerr << "---" << endl;
}

int Fuargs::add_action(const string& action_proto_s, action_func_t action_func)
{
  string action_proto, action_descr;
  if (size_t idx = action_proto_s.find("\n");
      idx != string::npos) {
    action_proto = string(action_proto_s, 0, idx);
    action_descr = string(action_proto_s, idx + 1);
  } else {
    action_proto = action_proto_s;
  }

  auto [action_name, action_args_list] = parse_action_proto(action_proto);
  if (auto it = action_protos.find(action_name);
      it != action_protos.end()) {
    ostringstream m; m << "action " << action_name << " was already added";
    throw runtime_error(m.str());
  }
  action_protos[action_name] = make_pair(action_args_list, action_descr);
  actions[action_name] = action_func;
  return 1;
}

bool Fuargs::verify_action_args(const string& action_name,
				const args& args)
{
  auto it = action_protos.find(action_name);
  if (it == action_protos.end()) {
    ostringstream m; m << "no such action: " << action_name;
    throw runtime_error(m.str());
  }

  vector<string> args_proto_list = (*it).second.first;
  return args.is_compatible_args_proto_list(args_proto_list);
}

void Fuargs::exec_actions(int argc, char** argv)
{ 
  Fuargs::argc = argc;
  Fuargs::argv = argv;

  if (argc < 2) {
    print_known_actions();
    exit(2);
  }
  
  ostringstream cmd_line_s;
  for (int i = 1; i < argc; i++) {
    cmd_line_s << argv[i];
    if (i + 1 < argc) {
      cmd_line_s << " ";
    }
  }

  auto [action, args] = parse_action(cmd_line_s.str());
  
  auto it = actions.find(action);
  if (it != actions.end()) {
    if (!verify_action_args(action, args)) {
      throw runtime_error("verify_action_args failed");
    }
    bool res = (*it).second(args);
    exit(res ? 0 : 1);
  } else {
    cerr << "unknown action: " << action << endl;
    print_known_actions();
    exit(3);
  }
}

void Fuargs::print_known_actions()
{
  for (auto [k, v]: action_protos) {
    cerr << k << endl;
    for (auto arg: v.first) {
      cerr << arg << endl;
    }
    cerr << v.second << endl;
  }
}     

// parsing functions

pair<string, vector<string>>
Fuargs::parse_action_proto(const string& action_proto)
{
  regex action_proto_re(R"D((\w+)\s*\[(.*)\])D");
  if (!regex_match(action_proto, action_proto_re)) {
    ostringstream m; m << "action proto " << action_proto << " fails to parse";
    throw runtime_error(m.str());
  }

  smatch m; regex_search(action_proto, m, action_proto_re);
  if (!(m.ready() && m.size() == 3)) {
    ostringstream m; m << "proto " << action_proto << " fails to parse";
    throw runtime_error(m.str());
  }

  string action_name = m[1].str();
  string action_proto_args = m[2].str();
  regex action_proto_args_re(R"D((\w+,)*(\w+)?)D");
  if (!regex_match(action_proto_args, action_proto_args_re)) {
    ostringstream m; m << "proto args invalid: " << action_proto_args;
    throw runtime_error(m.str());
  }

  auto action_proto_args_v = string_split(action_proto_args, ',');
  return make_pair(action_name, action_proto_args_v);
}

static Fuargs::args parse_action_args(const string& action_args_s)
{
  Fuargs::args ret;

  regex parsing_re(R"D((\w+)\s*=\s*([\w=./\-\$\{\}\"\']+|\[[\s\w=,/\.\-\$\{\}\"\']+\])\s*,?\s*)D");
  regex checking_re(R"D((\w+\s*=\s*([\w=./\-\$\{\}\"\']+|\[[\s\w=,/\.\-\$\{\}\"\']+\])\s*,?\s*)+)D");
  
  string s = action_args_s;
  for (smatch m; regex_search(s, m, parsing_re); s = m.suffix()) {
    cout << m.size() << " " << m[0] << "|  |" << m.suffix() << endl;
    if (m.size() == 3) {
      cout << "'" << m[1] << "' ==>> '" << m[2] << "'" << endl;
      string key(m[1].str());
      string value(m[2].str());
      if (auto it = ret.arg_map.find(key); it != ret.arg_map.end()) {
	ostringstream m; m << "dup arg key " << key;
	throw runtime_error(m.str());
      }
      ret.arg_map[key] = value;
    } else {
      cout << "parser failed" << endl;
      throw runtime_error("parser failed");
    }

    if (m.suffix().str().size() > 0) {
      if (!regex_match(m.suffix().str(), checking_re)) {
	cout << "parser failed: " << m.suffix() << endl;
	throw runtime_error("parser failed");
      }
    }
  }

  return ret;
}

pair<string, Fuargs::args> Fuargs::parse_action(const string& action_s)
{
  regex action_re(R"D((\w+)\s*\[(.*)\])D");
  if (!regex_match(action_s, action_re)) {
    ostringstream m; m << "can't parse " << action_s;
    throw runtime_error(m.str());
  }

  smatch m; regex_search(action_s, m, action_re);
  if (!(m.ready() && m.size() == 3)) {
    ostringstream m; m << "regex_serch failed on " << action_s;
  }
  
  auto action = m[1].str();
  auto action_args_s = m[2].str();
  cout << "action_args_s: '" << action_args_s << "'" << endl;
  auto action_args = parse_action_args(action_args_s);
  
  return make_pair(action, action_args);
}
