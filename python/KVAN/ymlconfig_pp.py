from io import StringIO
from .dvexpansion import *

class YMLConfigPP:
    def __init__(self, pathes):
        self.out_fd = StringIO()
        self.include_files = set()
        self.pp_pathes = []
        for p in pathes:
            self.pp_pathes.append(evaluate_dollar_var_expr(p))

    def find_yml_file(self, yml):
        ret = None
        for pp_path in self.pp_pathes:
            print("pp_path: ", pp_path)
            yml_fn = os.path.join(pp_path, yml)
            if os.path.exists(yml_fn):
                ret = yml_fn
                break
        return ret

    def get_pp_content(self):
        return self.out_fd.getvalue()
    
    def run_pp(self, yml_fn):
        fd = open(yml_fn, "r")
        
        self.out_fd.write(f"# {yml_fn}\n")
        while 1:
            l = fd.readline()
            if not l:
                break
            if l.find("!include") != -1:
                self.out_fd.write(f"# {l}")
                self.process_include(l)
            else:
                self.out_fd.write(l)
                
        self.out_fd.write("\n#\n")
        self.out_fd.write(f"# end of file {yml_fn}\n")

    def process_include(self, include_line):
        include_re = r"!include\s+<([\w+/\.\-]+)>"

        m = re.match(include_re, include_line)
        if m == None or len(m.groups()) != 1:
            raise Exception(f"YMLConfigPP::process_include: malformed line {include_line}")

        include_file = self.find_yml_file(m.groups()[0])
        if include_file == None:
            raise Exception(f"YMLConfigPP::process_include: can't resolve {include_line}")

        if not include_file in self.include_files:
            self.include_files.add(include_file)
            self.run_pp(include_file)
        
