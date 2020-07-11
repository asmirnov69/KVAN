#include <iostream>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/ymlconfig.h>
#include <kvan/ymlconfig-pp.h>
#include <kvan/string-utils.h>

ADD_ACTION("cpp[yml_fn,pp_pathes]", [](const Fuargs::args& args) {
    string yml_fn = args.get("yml_fn");
    vector pp_pathes = string_strip(string_split(args.get("pp_pathes"), ','));
    cout << "pp_pathes: " << string_join(pp_pathes, ';') << endl;

    YMLConfigPP pp(pp_pathes);
    pp.run_pp(args.get("yml_fn"));
    string pp_content;
    pp.get_pp_content(&pp_content);
    cout << pp_content << endl;
    return true;
  });

ADD_ACTION("parse_yml_file[yml_fn,pp_pathes]", [](const Fuargs::args& args) {
    string yml_fn = args.get("yml_fn");
    auto pp_pathes = string_strip(string_split(args.get("pp_pathes"), ','));
    cout << "pp_pathes: " << string_join(pp_pathes, ';') << endl;

    YMLConfig conf;
    conf.parse_file(yml_fn, pp_pathes);
    cout << "parse complete" << endl;
    conf.dump();
    cout << "--------" << endl;
      
    return true;
  });

auto parse_yml_proto = R"D(parse_yml[yml,pp_pathes]
lookup yml file in pp_pathes then parse it
)D";
ADD_ACTION(parse_yml_proto, [](const Fuargs::args& args) {
    auto pp_pathes = string_strip(string_split(args.get("pp_pathes"), ','));
    cout << "pp_pathes: " << string_join(pp_pathes, ';') << endl;
    YMLConfigPP pp(pp_pathes);
    auto yml_fn = pp.find_yml_file(args.get("yml"));
    if (yml_fn.second == false) {
      cerr << "can't find yml file " << args.get("yml") << endl;
      return false;
    }
    
    YMLConfig conf;
    conf.parse_file(yml_fn.first, pp_pathes);
    cout << "parse complete" << endl;
    conf.dump();
    cout << "--------" << endl;
      
  return true;
  });


int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}

  
