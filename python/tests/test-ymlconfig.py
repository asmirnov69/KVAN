import sys
sys.path.append("..")
import fuargs
from fuargs import config

@fuargs.action
def test_config():
    """
    this is a test of yml config
    """
    print(config)
    print(config.a)
    print(config.b)
    print(config.c.d, config.c.e)
    
if __name__ == "__main__":
    #print(sys.argv)
    config.init("./test.yml", pp_pathes=["."])
    fuargs.exec_actions(sys.argv[1:])
