import sys
from .fuargs import *
from .ymlconfig import *

@action
def test_actions(a,b,c):
    print("test_actions: ", a, b, c)

# instead of @fuargs.action
@action
def test_ymlconfig(yml_fn):
    print("test_ymlconfig")
    config.init(yml_fn, pp_pathes = ["."])
    print(config.as_json())
    config.print_compound_keys()

if __name__ == "__main__":
    exec_actions(sys.argv[1:])
