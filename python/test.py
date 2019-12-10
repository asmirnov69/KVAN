import sys, fuargs

@fuargs.action
def test_action(a, b, c):
    print "test action run", a, b, c

if __name__ == "__main__":
    fuargs.exec_actions(sys.argv[1:])
