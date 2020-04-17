import os.path, sys

class TopDir:
    def __init__(self, marker_file):
        self.marker_file = marker_file

    def topdir(self, pn = "."):
        prev_pn = None
        while 1:
            if prev_pn is not None and pn == prev_pn: # at the top
                raise Exception("topdir: no marker file found: %s" % self.marker_file)

            marker_pn = os.path.join(pn, self.marker_file)
            if os.path.exists(marker_pn):
                if not os.path.isfile(marker_pn):
                    raise Exception("topdir: name clash, marker file %s is directory or else" % marker_pn)
                return pn
            prev_pn = pn
            pn = os.path.dirname(pn)

def topdir(marker_file = 'TOP-DIR', verbose = False):
    o = TopDir(marker_file)    
    argv0 = sys.argv[0]
    if os.path.basename(argv0) == "ipython":
        pn = os.getcwd()
    else:
        pn = os.path.realpath(os.path.dirname(argv0))

    topdir_pn = o.topdir(pn)
    topdir_fn = os.path.join(topdir_pn, marker_file)
    if os.stat(topdir_fn).st_size > 0:
        # reading lines and insert them to sys.path
        ls = map(lambda x: x.strip(), open(topdir_fn, "r").readlines())
        for l in ls:
            if l[0] != '/':
                l = os.path.join(topdir_pn, l)
            if verbose: print("appending ", l, "to sys.path")
            sys.path.append(l)
    return topdir_pn

if __name__ == "__main__":
    print("__file__:", __file__)
    print(topdir(marker_file = "TOP-DIR"))
