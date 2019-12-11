# maybe it is easier to libyaml instead of ruamel
# C tutrorial on libyaml: https://www.wpsoftware.net/andrew/pages/libyaml.html
# python libyaml: https://pyyaml.org/wiki/PyYAML
#
#import ipdb
import sys
import os.path, StringIO
import ruamel.yaml
import ruamel
from collections import namedtuple, OrderedDict
import json
import pcpp

def isnamedtupleinstance(x):
    t = type(x)
    b = t.__bases__
    if len(b) != 1 or b[0] != tuple: return False
    f = getattr(t, '_fields', None)
    if not isinstance(f, tuple): return False
    return all(type(n) == str for n in f)

def make_nt__(d):
    m_values = []
    for k, v in d.items():
        #ipdb.set_trace()
        if isinstance(v, ruamel.yaml.comments.CommentedMap):
            m_values.append(make_nt__(v))
        elif isinstance(v, ruamel.yaml.comments.CommentedSeq):
            m_values.append(list(v))
        else:
            m_values.append(v)
    nt_ = namedtuple('nt', d.keys())
    return nt_(*m_values)

class MyPP(pcpp.Preprocessor):
    def on_error(self, file, line, msg):
        super(MyPP, self).on_error(file, line, msg)

class ConfigReader:
    def preprocess_(self, filename, pp_pathes):
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

        pp = pcpp.Preprocessor()
        if pp_pathes:
            for ppath in pp_pathes:
                pp.add_path(ppath)
        pp.parse(fd)
        ios = StringIO.StringIO()
        pp.write(ios); ios.seek(0)
        if pp.return_code > 0:
            raise Exception("Config::preprocess_ failed")
        return ios

    def run_pp(self, conf_file, pp_pathes):
        print "pp_pathes:", pp_pathes
        print "conf_file:", conf_file
        config_ios = self.preprocess_(conf_file, pp_pathes)
        print config_ios.getvalue()

    def read(self, conf_file, pp_pathes):
        config_ios = self.preprocess_(conf_file, pp_pathes)

        yaml_o = ruamel.yaml.YAML()
        yaml_o.duplicate_keys = False
        yml_conf = yaml_o.load(config_ios)
        _full_conf = make_nt__(yml_conf)
        return _full_conf

def print_keys__(parent_v, ls, curr_l):
    for f in parent_v._fields:
        v = parent_v.__getattribute__(f)
        if isnamedtupleinstance(v):
            curr_l.append(f)
            print_keys__(v, ls, curr_l)
            curr_l.pop()
        else:
            curr_l.append(f)
            ls.append(list(curr_l))
            curr_l.pop()
    return ls

def namedtuple_asdict(obj):
    if hasattr(obj, "_asdict"):
        return OrderedDict(zip(obj._fields, (namedtuple_asdict(item) for item in obj)))
    elif isinstance(obj, basestring):
        return obj
    elif hasattr(obj, "keys"):
        return OrderedDict(zip(obj.keys(), (namedtuple_asdict(item) for item in obj.values())))
    elif hasattr(obj, "__iter__"):
        return type(obj)((namedtuple_asdict(item) for item in obj))
    else:
        return obj

class Config:
    def __init__(self):
        self.config = None

    def init(self, conf_file, pp_pathes):
        cr = ConfigReader()
        self.config = cr.read(conf_file, pp_pathes)

    def __getattr__(self, an):
        return self.config.__getattribute__(an)

    def __dir__(self):
        return list(self.config._fields)

    def run_pp(self, conf_file, pp_pathes):
        cr = ConfigReader()
        cr.run_pp(conf_file, pp_pathes)

    def pprint(self):
        print self.config if self.config else '--no config loaded--'

    def print_keys(self):
        if self.config is None:
            print "--no config loaded--"
            return
        ls = print_keys__(self.config, [], [])
        print "\n".join([".".join(l) for l in ls])

    def as_json(self):
        return json.dumps(namedtuple_asdict(self.config))
    
config = Config()
