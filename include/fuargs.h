// -*- c++ -*-
#ifndef __FUARG_HH__
#define __FUARG_HH__

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
using namespace std;

class Fuargs
{
 public:
  class args {
  public:
    map<string, string> arg_map;
    string get(const char* argname) const {
      auto it = arg_map.find(argname);
      if (it == arg_map.end()) {
	ostringstream m; m << "no such arg: " << argname;
	throw runtime_error(m.str());
      }
      return (*it).second;
    }
  };

  typedef bool (*action_t)(const args&);
    
  static map<string, pair<action_t, vector<string>>> actions;
  static void add_action(const char* action_name, action_t action_func, const vector<string>& argnames);
  static void exec_actions(int argc, char** argv);

 private:
  static bool verbose;
  static string pending_action;
  static string pending_key, pending_value;
  static args pending_args;

  static bool printAction(const char* lexem, size_t len);
  static bool printKey(const char* lexem, size_t len);
  static bool printValue(const char* lexem, size_t len);
};

#endif
