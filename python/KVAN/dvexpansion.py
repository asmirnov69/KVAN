import os
import re
from .topdir import TopDir

def evaluate_dollar_var_expr(dv_expr):
    env_re = r"\$ENV\{(\w+)\}"
    s = dv_expr
    m = re.match(env_re, s)
    if m != None:
        for g in m.groups():
            env_var = g[0]
            env_var_value = os.environ[env_var]
            repl = "$ENV{" + env_var + "}"
            s = s.replace(repl, env_var_value)

    top_dir = TopDir.get().get_topdir()
    s = s.replace("${top-dir}", top_dir)

    l_etc_dir = os.path.join(top_dir, "etc")
    upper_etc_dir = os.path.realpath(os.path.join(top_dir, "..", "etc"))
    f = int(os.path.exists(l_etc_dir))
    f = f + int(os.path.exists(upper_etc_dir))
    if f > 1:
        raise Exception("both ${top-dir}/etc and ${top-dir}../etc exists")
    
    etc_dir = l_etc_dir if os.path.exists(l_etc_dir) else upper_etc_dir
    s = s.replace("${etc-dir}", etc_dir)
    
    return s
