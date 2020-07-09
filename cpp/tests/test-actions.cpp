#include <iostream>
#include <kvan/fuargs.h>

//auto action_proto = "action[hi,a,b]";
ADD_ACTION("action[hi,a,b]", [](const Fuargs::args& args) {
    cout << "test_action called" << endl;
    cout << "hi: " << args.get("hi") << endl;
    cout << "all done" << endl;
    return true;
  });

auto action2_proto = R"D(action2[]
action2 -- something really important 
to say
go here
)D";
ADD_ACTION(action2_proto, [](const Fuargs::args& args) {
    cout << "test_action2 called" << endl;
    cout << "all done" << endl;
    return true;
  });

int main(int argc, char* argv[])
{
  Fuargs::exec_actions(argc, argv);
}
