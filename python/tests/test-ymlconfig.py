#import ipdb
import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(sys.argv[0]), ".."))
from KVAN import fuargs
from KVAN import ymlconfig
from KVAN.ymlconfig import config

@fuargs.action
def test_config(yml_fn):
    """
    this is a test of yml config
    """
    config.init(yml_fn, pp_pathes=["."])
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
    
if __name__ == "__main__":
    #print(sys.argv)
    #config.init("./test.yml", pp_pathes=["."])
    fuargs.exec_actions(sys.argv[1:])
    #test_config(sys.argv[1])
