#include <iostream>
#include "fuargs.h"

bool test_action(const map<string, string>& args) {
  cout << "test_action called" << endl;
  for (auto it = args.begin(); it != args.end(); ++it) {
    cout << (*it).first << " " << (*it).second << endl;
  }

  cout << "all done" << endl;
}

int main(int argc, char* argv[])
{
  Fuargs::add_action("test_action", test_action);
  Fuargs::exec_actions(argc, argv);
}
