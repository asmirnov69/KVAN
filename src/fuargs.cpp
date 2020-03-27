#include <fuargs.h>

#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <bnflite.h>
using namespace bnf;

namespace Fuargs {
  bool ACTIONS::verbose = false;
  map<string, unique_ptr<ACTION>> ACTIONS::actions;
  string ACTIONS::pending_action;
  string ACTIONS::pending_key;
  string ACTIONS::pending_value;
  args ACTIONS::pending_args;

  ARG::ARG(ACTION* action, const char* arg_name, const char* arg_descr)
  {
    this->action = action;
    this->arg_name = arg_name;
    this->arg_descr = arg_descr;
    this->action->actual_args.push_back(this);
  }

  void ACTION::init_actual_args(const args& call_args)
  {
    for (auto actual_arg: actual_args) {
      auto it = call_args.arg_map.find(actual_arg->arg_name);
      if (it == call_args.arg_map.end()) {
	ostringstream m; m << "missing expected arg " << actual_arg->arg_name;
	throw runtime_error(m.str());
      }
      actual_arg->arg_value = (*it).second;
    }
  }
  
  bool ACTIONS::printAction(const char* lexem, size_t len)
  {
    if (verbose) cout << "printAction: " << string(lexem, len) << endl;
    pending_action = string(lexem, len);
    return true;
  }
  
  bool ACTIONS::printKey(const char* lexem, size_t len)
  {
    if (verbose) cout << "printKey: " << string(lexem, len) << endl;
    pending_key = string(lexem, len);
    return true;
  }

  bool ACTIONS::printValue(const char* lexem, size_t len)
  {
    if (verbose) cout << "printValue: " << string(lexem, len) << endl;
    pending_value = string(lexem, len);
    pending_args.arg_map[pending_key] = pending_value;
    return true;
  }

  void ACTIONS::print_known_actions()
  {
    for (auto it = actions.begin(); it != actions.end(); ++it) {
      ostringstream argnames_s;
      for (size_t j = 0; j < (*it).second->actual_args.size(); j++) {
	argnames_s << (*it).second->actual_args[j]->arg_name;
	if (j + 1 < (*it).second->actual_args.size()) {
	  argnames_s << ",";
	}
      }
      
      cerr << (*it).first << "[" << argnames_s.str() << "]" << endl;
    }
  }
  
  void ACTIONS::exec_actions(int argc, char** argv)
  { 
    Token Alphanumeric('_');    // start declare one element of "sequence of alphanumeric characters"
    Alphanumeric.Add('/');
    Alphanumeric.Add('-');
    Alphanumeric.Add('.');
    Alphanumeric.Add('~');
    Alphanumeric.Add('0', '9'); // appended numeric part
    Alphanumeric.Add('a', 'z'); // appended alphabetic lowercase part
    Alphanumeric.Add('A', 'Z'); // appended alphabetic capital part
    Lexem NAME = Series(1, Alphanumeric); // declare "sequence of alphanumeric characters"
    Lexem KEY = Series(1, Alphanumeric);
    Lexem VALUE = Series(1, Alphanumeric);
    Rule Kv = KEY + printKey + "=" + VALUE + printValue;
    Rule Kvpairs = (Kv + Repeat(0, "," + Kv)) | Null();
    Rule Action = NAME + printAction + "[" + Kvpairs + "]";
    Rule ACTIONCHAIN = Action + Repeat(0, "," + Action);
    
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
    
    const char* test = strdup(cmd_line_s.str().c_str()); //"[0]amerge=0=5,, c1";
    if (verbose) cout << "test: " << test << endl;  
    const char* pstr = 0;
    int stat = Analyze(ACTIONCHAIN, test, &pstr); // Start parsing
    
    if (stat > 0) {
      if (verbose) cout << "Passed" << endl;
      auto it = actions.find(pending_action);
      if (it != actions.end()) {
	// need to setup all args here
	//pending_args.dump();
	(*it).second->init_actual_args(pending_args);
	(*it).second->action();
      } else {
	cerr << "unknown action: " << pending_action << endl;
	cerr << "known actions: " << endl;
	print_known_actions();
	//throw runtime_error("action not found");
	exit(3);
      }
    } else {
      cout << "Failed, stopped at=" //%.40s\n status = 0x%0X,  flg = %s%s%s%s%s%s%s%s\n",
	   << (pstr?pstr:"") << " " << stat
	   << (stat&eOk?"eOk":"Not")
	   << (stat&eRest?", eRest":"")
	   << (stat&eOver?", eOver":"")
	   << (stat&eEof?", eEof":"")
	   << (stat&eBadRule?", eBadRule":"")
	   << (stat&eBadLexem?", eBadLexem":"")
	   << (stat&eSyntax?", eSyntax":"")
	   << (stat&eError?", eError":"")
	   << endl;
    }
  }
}
