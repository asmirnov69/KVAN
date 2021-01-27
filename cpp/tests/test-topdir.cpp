#include <iostream>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/topdir.h>
#include <kvan/string-utils.h>

ADD_ACTION("topdir[]", [](const Fuargs::args&) {
    auto top_dir = TopDir::get()->get_topdir();
    cout << "top_dir: " << TopDir::get()->get_topdir() << endl;
    return true;
  });

ADD_ACTION
("dollar_var[x]",
 R"D({argv0} dollar_var[x= [\"\'\$ENV\{HOME\}\'\",top-dir=\$\{top-dir\}]])D",
 [](const Fuargs::args& args) {
  auto x = args.get("x");
  cout << "x: " << x << endl;
  cout << evaluate_dollar_var_expr(x) << endl;
  return true;
 });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv, false, false);
}
