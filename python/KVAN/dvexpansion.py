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
            
    s = s.replace("${top-dir}", TopDir.get().get_topdir())
    return s
