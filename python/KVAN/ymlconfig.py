#import ipdb
import sys
import os.path, io, datetime
import yaml
from collections import namedtuple, OrderedDict
import json
from .ymlconfig_pp import *
from .dvexpansion import evaluate_dollar_var_expr

def isnamedtupleinstance(x):
    t = type(x)
    b = t.__bases__
    if len(b) != 1 or b[0] != tuple: return False
    f = getattr(t, '_fields', None)
    if not isinstance(f, tuple): return False
    return all(type(n) == str for n in f)

def make_nt__(d):
    #ipdb.set_trace()
    if not (isinstance(d, dict) or isinstance(d, list)):
        return d
    
    m_values = []
    for k, v in d.items():
        #ipdb.set_trace()
        if isinstance(v, dict):
            m_values.append(make_nt__(v))
        elif isinstance(v, list):
            res_l = []
            for el in v:
                res_l.append(make_nt__(el))
            m_values.append(res_l)
        elif isinstance(v, str):
            new_v = evaluate_dollar_var_expr(v)
            m_values.append(new_v)
        else:
            m_values.append(v)
                
    nt_ = namedtuple('nt', d.keys())
    return nt_(*m_values)

class ConfigReader:
    def run_pp(self, conf_file, pp_pathes):
        print("pp_pathes:", pp_pathes)
        print("conf_file:", conf_file)
        pp = YMLConfigPP()
        pp.add_pp_pathes(pp_pathes)
        pp.parse(filename)
        
        config_ios = self.preprocess_(conf_file, pp_pathes)
        print(config_ios.getvalue())

    def read(self, conf_file, pp_pathes):
        pp = YMLConfigPP(pp_pathes)
        pp.run_pp(conf_file)
        pp_fd = StringIO(pp.get_pp_content())
        
        yml_conf = yaml.load(pp_fd, Loader = yaml.FullLoader)
        _full_conf = make_nt__(yml_conf)
        return _full_conf

def print_keys__(parent_v, ls, curr_l):
    #ipdb.set_trace()
    for f in parent_v._fields:
        v = getattr(parent_v, f)
        if isnamedtupleinstance(v):
            curr_l.append(f); ls.append(list(curr_l))
            print_keys__(v, ls, curr_l)
            curr_l.pop()
        elif isinstance(v, list):
            curr_l.append(f)
            for i in range(len(v)):
                #ipdb.set_trace()
                if isnamedtupleinstance(v[i]) or isinstance(v[i], list):
                    curr_l.append("[%d]" % i); ls.append(list(curr_l))
                    print_keys__(v[i], ls, curr_l)
                    curr_l.pop()
            curr_l.pop()
        else:
            curr_l.append(f)
            ls.append(list(curr_l))
            curr_l.pop()
  
    return ls

def namedtuple_asdict(obj):
    if hasattr(obj, "_asdict"):
        return OrderedDict(zip(obj._fields, (namedtuple_asdict(item) for item in obj)))
    elif isinstance(obj, str):
        return obj
    elif isinstance(obj, datetime.date):
        return obj.strftime("%Y-%m-%d")
    elif hasattr(obj, "keys"):
        return OrderedDict(zip(obj.keys(), (namedtuple_asdict(item) for item in obj.values())))
    elif hasattr(obj, "__iter__"):
        return type(obj)((namedtuple_asdict(item) for item in obj))
    else:
        return obj

class Config:
    def __init__(self):
        self.config = None

    def parse_file(self, conf_file, pp_pathes):
        cr = ConfigReader()
        self.config = cr.read(conf_file, pp_pathes)

    def parse(self, conf_yml, pp_pathes):
        pp = YMLConfigPP(pp_pathes)
        conf_file = pp.find_yml_file(conf_yml)
        if conf_file == None:
            raise Exception(f"Config::parse: can't find yml file {conf_yml}")
        self.parse_file(conf_file, pp_pathes)
        
    def __getattr__(self, an):
        if hasattr(self.config, an):
            return self.config.__getattribute__(an)
        return object.__getattribute__(self, an)

    def __dir__(self):
        return list(self.config._fields)

    def run_pp(self, conf_file, pp_pathes):
        cr = ConfigReader()
        cr.run_pp(conf_file, pp_pathes)

    def pprint(self):
        print(self.config if self.config else '--no config loaded--')

    def get_compound_keys(self):
        return print_keys__(self.config, [], [])
    
    def print_compound_keys(self):
        if self.config is None:
            print("--no config loaded--")
            return
        ls = self.get_compound_keys()
        print("\n".join(["'" + ".".join(l) + "'" for l in ls]))        

    def as_json(self):
        return json.dumps(namedtuple_asdict(self.config))
    
config = Config()
