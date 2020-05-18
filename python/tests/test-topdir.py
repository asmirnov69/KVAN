import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(sys.argv[0]), ".."))
from KVAN import topdir
topdir(marker_file = 'marker.txt')

if __name__ == "__main__":
    print("all done")
    
