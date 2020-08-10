#include <iostream>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/sigio-logger.h>

ADD_ACTION("test_logger[]", [](const Fuargs::args&) {
    kvan::logger::setup("./logger.txt");
    cout << "this a test" << endl;
    cerr << "cerr test" << endl;    
    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}
