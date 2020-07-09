#include <iostream>
#include <kvan/fuargs.h>

class test_action : public Fuargs::ACTION {
  Fuargs::ARG hi{this, "hi", "this is an important param"};
  
  bool action()
  {
    cout << "test_action called" << endl;
    cout << "hi: " << hi.get() << endl;
    cout << "all done" << endl;
    return true;
  }
};

class test_action2 : public Fuargs::ACTION {
  bool action()
  {
    cout << "test_action2 called" << endl;
    cout << "all done" << endl;
    return true;
  }
};

int main(int argc, char* argv[])
{
  Fuargs::add_action<test_action>("test_action");
  Fuargs::add_action<test_action2>("test_action2");
  Fuargs::exec_actions(argc, argv);
}
