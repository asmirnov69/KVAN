
class MyPP:
    def add_pp_pathes(self, pathes):
        pass

    def find_yml_file(self, yml):
        fd = None
        if os.path.exists(filename):
            fd = open(filename)
        elif pp_pathes:
            for ppath in pp_pathes:
                mod_fn = os.path.join(ppath, os.path.basename(filename))
                if os.path.exists(mod_fn):
                    fd = open(mod_fn)
                    break
        if fd is None:
            raise Exception("can't locate %s" % filename)
        
    
    def parse(self, fn):
        pass
    
