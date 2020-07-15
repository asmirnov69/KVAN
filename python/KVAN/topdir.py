import os.path, sys

class TopDirImpl:
    def __init__(self, marker_file = "TOP-DIR"):
        self.marker_file = marker_file
        self.topdir = None
        argv0 = sys.argv[0]
        if os.path.basename(argv0) in ["ipython", "ymlconfig"]:
            pn = os.getcwd()
        else:
            pn = os.path.realpath(os.path.dirname(argv0))

        prev_pn = None
        while 1:
            if prev_pn is not None and pn == prev_pn: # at the top
                raise Exception("topdir: no marker file found: %s" % self.marker_file)

            marker_pn = os.path.join(pn, self.marker_file)
            if os.path.exists(marker_pn):
                if not os.path.isfile(marker_pn):
                    raise Exception("topdir: name clash, marker file %s is directory or else" % marker_pn)
                self.topdir = pn
                break
            prev_pn = pn
            pn = os.path.dirname(pn)

    def setup_syspath(self):
        topdir_fn = os.path.join(self.topdir, self.marker_file)
        if os.stat(topdir_fn).st_size > 0:
            # reading lines and insert them to sys.path
            ls = map(lambda x: x.strip(), open(topdir_fn, "r").readlines())
            for l in ls:
                if l[0] != '/':
                    l = os.path.join(self.topdir, l)
                    print("appending ", l, "to sys.path")
                    sys.path.append(l)

    def get_topdir(self):
        return self.topdir
                    
class TopDir:
    __instance = None
    @staticmethod
    def get():
        if TopDir.__instance == None:
            TopDir()
        return TopDir.__instance

    def __init__(self):
        if TopDir.__instance != None:
            raise Exception("This class is singleton")
        else:
            TopDir.__instance = TopDirImpl()

def setup_syspath():
    TopDir.get().setup_syspath()

def get_topdir():
    return TopDir.get().get_topdir()

