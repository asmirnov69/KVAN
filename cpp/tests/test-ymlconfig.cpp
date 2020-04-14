#include <iostream>
using namespace std;

#include <fuargs.h>
#include <ymlconfig.h>

class cpp : public Fuargs::ACTION {
  Fuargs::ARG yml_fn{this, "yml_fn"};

  bool action() {
    YMLConfig conf(true, false);
    string out, out_err;
    conf.run_preprocessor(yml_fn.get().c_str(),
			  vector<string>{"."},
			  &out, &out_err);
    cerr << out_err << endl;
    cerr << out << endl;

    return true;
  }
  
};

class parse_yml : public Fuargs::ACTION {
  Fuargs::ARG yml_fn{this, "yml_fn"};

  bool action() {
    YMLConfig conf(true, false);
    conf.parse(yml_fn.get().c_str(), {"."});
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
  }
};

int main(int argc, char** argv)
{
  Fuargs::add_action<parse_yml>("parse_yml");
  Fuargs::add_action<cpp>("cpp");
  Fuargs::exec_actions(argc, argv);
}

  
