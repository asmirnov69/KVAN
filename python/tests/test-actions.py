import sys, fuargs
sys.path.append("..")

@fuargs.action
def test_action(a, b, c):
    """
    this is just a test
    """
    print "test action run", a, b, c

if __name__ == "__main__":
    #print sys.argv
    fuargs.exec_actions(sys.argv[1:])
