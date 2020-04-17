import sys
sys.path.append("..")
from KVAN import fuargs

@fuargs.action
def test_fuargs(a, b, c):
    """
    this is just a test
    """
    print("test action run", a, b, c)

if __name__ == "__main__":
    #print sys.argv
    fuargs.exec_actions(sys.argv[1:])
