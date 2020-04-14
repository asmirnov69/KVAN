// -*- c++ -*-
#ifndef __FUARG_HH__
#define __FUARG_HH__

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <memory>
using namespace std;

namespace Fuargs
{
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

    void dump() {
      cerr << "args dump:" << endl;
      for (auto& [k, v]: arg_map) {
	cerr << k << ": " << v << endl;
      }
      cerr << "---" << endl;
    }
  };

  class ARG;
  class ACTION {
  public:
    vector<ARG*> actual_args;
    void init_actual_args(const args& call_args);
    virtual bool action() = 0;
  };

  class ARG {
  public:
    ACTION* action;
    string arg_name, arg_descr;
    string arg_value;
    ARG(ACTION*, const char* arg_name, const char* arg_descr = nullptr);
    string get() { return arg_value; }
  };

  class ACTIONS {
  private:
    static bool verbose;
    static string pending_action;
    static string pending_key, pending_value;
    static args pending_args;
    
    static bool printAction(const char* lexem, size_t len);
    static bool printKey(const char* lexem, size_t len);
    static bool printValue(const char* lexem, size_t len);

  public:
    static map<string, unique_ptr<ACTION>> actions;
    static void exec_actions(int argc, char** argv);
    static void print_known_actions();
  };
  
  template <class ACTION>
  static void add_action(const char* action_name) {
    auto it = ACTIONS::actions.find(action_name);
    if (it != ACTIONS::actions.end()) {
      ostringstream m;
      m << "such action already added: " << action_name;
      throw runtime_error(m.str());
    }

    auto p = make_unique<ACTION>();
    ACTIONS::actions[action_name] = move(p);
  }

  inline void exec_actions(int argc, char** argv) {
    ACTIONS::exec_actions(argc, argv);
  }
};

#endif
