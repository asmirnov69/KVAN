#include <iostream>
using namespace std;

#include <kvan/fuargs.h>
#include <kvan/ymlconfig.h>

ADD_ACTION("parse_yml[yml_fn]", [](const Fuargs::args& args) {
    YMLConfig conf("", true, false);
    conf.parse(args.get("yml_fn").c_str(), {"."});
    cout << "parse complete" << endl;
    conf.dump();
    cout << "--------" << endl;
    
#if 0
    {
      auto c_tracer = conf.get_config("simulation.tracer");
      c_tracer.dump();
      cout << "enabled: " << c_tracer.get("enabled") << endl;
    }
    cout << "---" << endl;
    {
      auto c_tracer = conf.get_config("simulation");
      c_tracer.dump();
      cout << "tracer.enabled: " << c_tracer.get("tracer.enabled") << endl;
    }

#if 0
    cout << "---" << endl;
    {
      auto c_tracer = conf.get_config("simulatio");
      c_tracer.dump();
      cout << "tracer.enabled: " << c_tracer.get("tracer.enabled") << endl;
    }
#endif

    cout << "---" << endl;
    {
      auto strat_conf = conf.get_config("StrategyFactory");
      cout << "strat_conf:" << endl;
      strat_conf.dump();
      cout << "==" << endl;
      strat_conf.get_config("test_strategy").dump();
    }
#endif
  
    return 0;
  });

ADD_ACTION("cpp[yml_fn]", [](const Fuargs::args& args) {
    string yml_fn = args.get("yml_fn");
    YMLConfig conf("", true, false);
#if 0
    string out;
    string out_err;
#else
    string out, out_err; // CPP BUG!
#endif
    
    conf.run_preprocessor(args.get("yml_fn").c_str(),
			  vector<string>{"."},
			  &out, &out_err);
    cerr << out_err << endl;
    cerr << out << endl;
    return true;
  });

int main(int argc, char** argv)
{
  Fuargs::exec_actions(argc, argv);
}

  
