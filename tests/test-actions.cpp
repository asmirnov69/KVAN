#include <iostream>
#include "fuargs.h"

bool test_action(const Fuargs::args& args)
{
  cout << "test_action called" << endl;
  cout << "hi: " << args.get("hi") << endl;
  cout << "all done" << endl;
  return true;
}

int main(int argc, char* argv[])
{
  Fuargs::add_action("test_action", test_action, {"hi"});
  Fuargs::exec_actions(argc, argv);
}
