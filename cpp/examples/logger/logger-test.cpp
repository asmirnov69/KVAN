#include <iostream>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/sigio-logger.h>

ADD_ACTION("test_logger[console]", [](const Fuargs::args& args) {
    bool output_to_console = args.get("console") == "true";
    cout << "output_to_console: " << output_to_console << endl;
    kvan::logger::setup("./logger.txt", output_to_console);
    cout << "this a test" << endl;
    cerr << "cerr test" << endl;    
    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}
