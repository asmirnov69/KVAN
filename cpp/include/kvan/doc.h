/*! \class Fuargs "include/kvan/fuargs.h"
\brief Fuargs -- functional-style command-line arguments parser

Fuargs is command-line arguments parser. To have it working it is neccesary to register action function using action proto string:

ADD_ACTION("doit[arg1,arg2]", [](const Fuarg::args& args)
{
  cout << "action doit starts,"; args.dump(cout); cout << endl;
  cout << "arg1: " << args.get("arg1") << endl;
  cout << "arg2: " << args.get("arg2") << endl;
  cout << "end of action" << endl;
  return 0;
}

Action proto string may have additional description of action. It is stored starting from second line of action proto:

auto doit_more_proto = R"D(doit_more[arg1,arg2,arg3]
the full descption of doit_more action is following:
....
)D";
ADD_ACTION(doit_more_proto, [](const Fuargs::args&) {
  ... TBC ...
});

*/

