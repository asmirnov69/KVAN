// -*- c++ -*-
#ifndef __FUARG_HH__
#define __FUARG_HH__

#include <map>
#include <string>
#include <functional>
using namespace std;

class Fuargs
{
public:
  static int argc;
  static char** argv;
  
  struct args {
    map<string, string> arg_map;
    string get(const char* argname) const;
    void dump() const;
    bool is_compatible_args_proto_list(const vector<string>&) const;
  };

  typedef function<bool(const Fuargs::args&)> action_func_t;

  static int add_action(const string& action_proto, action_func_t);
  static void print_known_actions();
  static void exec_actions(int argc, char** argv);

private:
  // first: action name, second: all action args
  static pair<string, vector<string>>
  parse_action_proto(const string& action_proto);
  // first: action name, second: args map
  static pair<string, struct args> parse_action(const string& action_s);
  // verifies that given action and its args do match proto
  static bool verify_action_args(const string& action, const args& args);

  // key: action name, value: (action arg names, action descr)
  static map<string, pair<vector<string>, string>> action_protos;
  // key: action name, value: function to be called
  static map<string, action_func_t> actions;
};

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define ADD_ACTION(x, ...) static int TOKENPASTE2(Unique_, __LINE__) = Fuargs::add_action((x), __VA_ARGS__)


#endif
