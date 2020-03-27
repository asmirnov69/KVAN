#include <fuargs.h>

#include <string>
#include <iostream>
#include <sstream>
using namespace std;

#include <stdlib.h>
#include <bnflite.h>
using namespace bnf;

bool Fuargs::verbose = false;
map<string, pair<Fuargs::action_t, vector<string>>> Fuargs::actions;
string Fuargs::pending_action;
string Fuargs::pending_key;
string Fuargs::pending_value;
Fuargs::args Fuargs::pending_args;

void Fuargs::add_action(const char* action_name, action_t action_func,
			const vector<string>& argnames)
{
  auto fres = actions.find(action_name);
  if (fres != actions.end()) {
    ostringstream m;
    m << "such action already added: " << action_name;
    throw runtime_error(m.str());
  }
  actions[action_name] = make_pair(action_func, argnames);
}

bool Fuargs::printAction(const char* lexem, size_t len)
{
  if (verbose) cout << "printAction: " << string(lexem, len) << endl;
  pending_action = string(lexem, len);
  return true;
}

bool Fuargs::printKey(const char* lexem, size_t len)
{
  if (verbose) cout << "printKey: " << string(lexem, len) << endl;
  pending_key = string(lexem, len);
  return true;
}

bool Fuargs::printValue(const char* lexem, size_t len)
{
  if (verbose) cout << "printValue: " << string(lexem, len) << endl;
  pending_value = string(lexem, len);
  pending_args.arg_map[pending_key] = pending_value;
  return true;
}

void Fuargs::exec_actions(int argc, char** argv)
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
  Rule Kv = KEY + Fuargs::printKey + "=" + VALUE + Fuargs::printValue;
  Rule Kvpairs = Kv + Repeat(0, "," + Kv);
  Rule Action = NAME + Fuargs::printAction + "[" + Kvpairs + "]";
  Rule ACTIONCHAIN = Action + Repeat(0, "," + Action);

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
      (*it).second.first(pending_args);
    } else {
      cerr << "known actions: " << endl;
      for (auto it = actions.begin(); it != actions.end(); ++it) {
	ostringstream argnames_s;
	for (size_t j = 0; j < (*it).second.second.size(); j++) {
	  argnames_s << (*it).second.second[j];
	  if (j + 1 < (*it).second.second.size()) {
	    argnames_s << ",";
	  }
	}
	cerr << (*it).first << "[" << argnames_s.str() << "]" << endl;
      }
      throw runtime_error("action not found");
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

