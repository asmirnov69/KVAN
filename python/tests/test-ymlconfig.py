#import ipdb
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(sys.argv[0]), ".."))
from KVAN import fuargs
from KVAN import ymlconfig, ymlconfig_pp
from KVAN.ymlconfig import config

@fuargs.action
def cpp(yml_fn, pp_pathes):
    pp = ymlconfig_pp.YMLConfigPP(pp_pathes)
    pp.run_pp(yml_fn)
    print(pp.get_pp_content())
    
@fuargs.action
def parse_yml_file(yml_fn, pp_pathes):
    """
    this is a test of yml config
    """
    config.parse_file__(yml_fn, pp_pathes)
    print(config.as_json())
    #ipdb.set_trace()
    print("\n".join([".".join(l) for l in config.get_compound_keys()]))
    print("-------------")
    config.print_compound_keys()
    print("-------------------")
    config.pprint()
    #print(config.a)
    #print(config.b)
    #print(config.c.d, config.c.e)

@fuargs.action
def use_config():
    config.pprint();
    
if __name__ == "__main__":
    config.parse("test-config.yml")
    fuargs.exec_actions(sys.argv[1:])
