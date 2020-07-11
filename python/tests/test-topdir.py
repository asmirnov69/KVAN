import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(sys.argv[0]), ".."))
from KVAN import topdir

if __name__ == "__main__":
    print("topdir: ", topdir.get_topdir())
    print("all done")
