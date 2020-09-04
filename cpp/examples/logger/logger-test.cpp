#include <iostream>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/logger.h>

ADD_ACTION("test_logger[]", [](const Fuargs::args& args) {
    //bool output_to_console = args.get("console") == "true";
    //cout << "output_to_console: " << output_to_console << endl;
    //kvan::logger::setup("./logger.txt", output_to_console);

    for (int i = 0; i < 5; i++) {
      cout << "this " << "a " << "test" << endl;
      cerr << "cerr " << "test" << endl;
    }
    return true;
  });

ADD_ACTION("test_logger_crash[]", [](const Fuargs::args& args) {
    //bool output_to_console = args.get("console") == "true";
    //cout << "output_to_console: " << output_to_console << endl;
    //kvan::logger::setup("./logger.txt", output_to_console);

    for (int i = 0; i < 5; i++) {
      cout << "this " << "a " << "test" << endl;
      cerr << "cerr " << "test" << endl;
    }

    int* a;
    for (int i = 0; i < 5; i++) {
      cout << "this " << "a " << a[i] << "test" << endl;
      cerr << "cerr " << "test " << a[i] << endl;
      throw runtime_error("error");
      a[i] = i;
    }
    return true;
  });

int main(int argc, char** argv)
{
  kvan::logger_setup(argv[0]);
  kvan::sigio_setup(argv[0]);
  Fuargs::exec_actions(argc, argv);
}
