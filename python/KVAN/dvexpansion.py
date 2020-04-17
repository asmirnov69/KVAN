import os

def evaluate_dollar_var_expr(dv_expr):
    prefix_end_index = dv_expr.find("{")
    prefix = dv_expr[:prefix_end_index]
    arg = dv_expr[prefix_end_index+1:-1]

    ret = None
    if prefix == "ENV":
        if arg in os.environ:
            ret = os.environ[arg]
        else:
            raise Exception("evaluate_dollar_var_expr: can't find env var %s" % arg)
    elif len(prefix) > 0:
        raise Exception("unknown dollar var prefix: %s, should be $ENV{var-name}" % prefix)
    else:
        raise Exception("prefix was empty in expr %s, should be ${ENV{var-name}" % dv_expr)

    return ret

def string_replace(v, b_idx, e_idx, new_v):
    return v[:b_idx] + new_v + v[e_idx:]

def do_dollar_value_expansion(v):
    new_v = v
    while 1:
        start_dv_index = 0
        end_dv_index = -1
        start_dv_index = new_v.find("$", end_dv_index + 1)
        if start_dv_index == -1:
            break
        end_dv_index = new_v.find("}", start_dv_index + 1)
        if end_dv_index == -1:
            break
        dollar_var_expr = new_v[start_dv_index+1:end_dv_index+1]
        dollar_var_value = evaluate_dollar_var_expr(dollar_var_expr)
        print("dollar_var_expr:", dollar_var_expr)
        print("dollar_var_value:", dollar_var_value)
        new_v = string_replace(new_v, start_dv_index, end_dv_index+1, dollar_var_value)
    return new_v

if __name__ == "__main__":
    print(do_dollar_value_expansion("$ENV{HOME}$ENV{USER} -- $ENV{TERM}"))
