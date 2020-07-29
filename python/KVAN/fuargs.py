#import ipdb
import sys
from collections import OrderedDict as odict
import inspect
from pyparsing import *

def build_parser():
    action = Word(alphanums + '_').setResultsName('action')
    arg_name = Word(alphanums + '_').setResultsName('arg_name')

    value_class = Word(printables, excludeChars = "[],").setResultsName('arg_value')
    value_list_class = delimitedList(value_class, ',').setResultsName('arg_value')
    arg_value = MatchFirst([value_class, Suppress('[') + value_list_class + Suppress(']')])
    
    arg_pair = Group(arg_name + Suppress('=') + arg_value)
    arg_pairs = delimitedList(arg_pair, ',').setResultsName('arg_pairs')
    action_w_args = Group(action + Suppress('[') + Optional(arg_pairs) + Suppress(']')).setResultsName('action_w_args')
    
    help_opt = oneOf("-h --help").setResultsName("help")
    console_opt = oneOf("-q --no-console").setResultsName('no_console')
    log_opt = oneOf("-Q --no-log").setResultsName("no_log")
    debug_opt = oneOf("-d --debug").setResultsName("debug")
    show_opt = oneOf("-s --show").setResultsName("show")
    dump_config_fn = Literal('--dump-config').setResultsName('do_dump_config') \
                     + Optional(Suppress('=') + Word(alphanums).setResultsName('config_dump_fn'))
    cmd_parser = Or([dump_config_fn,
                     Each([Optional(help_opt), Optional(console_opt), Optional(log_opt),
                           Optional(debug_opt), Optional(show_opt)]) \
                     + ZeroOrMore(action_w_args).setResultsName('cmds')])
    return cmd_parser

def print_usage(argv0, out_fd):
    print("{argv0} -h|--help".format(argv0 = argv0), file = out_fd)
    print("{argv0} --dump-config[=<file>]".format(argv0 = argv0), file = out_fd)
    print("{argv0} [options] actions".format(argv0 = argv0), file = out_fd)
    print("", file = out_fd)
    print("options:", file = out_fd)
    print("-h or --help -- show help screen and exits", file = out_fd)
    print("-d or --debug -- run actions with debugger being enabled", file = out_fd)
    print("-q or --no-console -- no console output during actions run", file = out_fd)
    print("-Q or --no-log -- no log output during actions run", file = out_fd)
    print("-s or --show -- print parsed options and processed config then exits", file = out_fd)
    print("--dump-config or --dump-config=<output-file> -- ... then exits", file = out_fd)
    sys.exit(23)

def dump_config(out_fn):
    out_fd = open(out_fn, 'r') if out_fn else sys.stdout
    print("config dump", file = out_fd)
    sys.exit(23)
    
def show_opts_actions(opts, actions, out_fd):
    print("opts:", file = out_fd)
    for k in opts.keys():
        print(" ", k, ":", opts[k], file = out_fd)
    print("actions:", file = out_fd)
    for action in actions:
        print(" action:", action.action, file = out_fd)
        for arg in action.args:
            print("  ", arg, "==>", action.args[arg], file = out_fd)
    sys.exit(3)
    
class Action:
    def __init__(self, action):
        self.action = action
        self.args = odict()

    def append_arg(self, arg_name, arg_value):
        self.args[arg_name] = arg_value
    
def parse_cmdline(cmdline_args):
    #dbg.set_trace()
    cmdline = " ".join(cmdline_args)

    cmd_parser = build_parser()
    tokens = cmd_parser.parseString(cmdline, parseAll = True)

    opts = odict()
    opts["help"] = 'help' in tokens
    opts["no_console"] = 'no_console' in tokens
    opts["no_log"] = 'no_log' in tokens
    opts["debug"] = 'debug' in tokens
    opts["show"] = 'show' in tokens
    opts["do_dump_config"] = 'do_dump_config' in tokens
    opts["config_dump_fn"] = tokens.config_dump_fn if 'config_dump_fn' in tokens else None

    actions = []
    if 'cmds' in tokens:
        for action_w_args in tokens.cmds:
            action = Action(action_w_args.action)
            for x in action_w_args.arg_pairs:
                arg_name = x.arg_name
                arg_value = x.arg_value if isinstance(x.arg_value, str) else x.arg_value.asList()
                action.append_arg(arg_name, arg_value)
                #print(arg_name, type(arg_name), arg_value, type(arg_value))
                #print('.............')
            actions.append(action)

    return opts, actions

# ...........................................................

all_actions = odict()

def reg_action(func):
    func_def = {name:data for name, data in inspect.getmembers(func)}
    #ipdb.set_trace()
    func_module = func_def['__module__']
    func_name = func_def['__name__']
    reg_func_name = func_name if func_module == '__main__' else func_module + '.' + func_name
    func_sig = inspect.signature(func)
    all_actions[reg_func_name] = (func, func_sig)

# this is decorator
def action(func):
    reg_action(func)
    return func

def verify_action_args(action_args, func_sig):
    #ipdb.set_trace()
    required_params = set([k for k, v in func_sig.parameters.items() if v.default == v.empty])
    all_params = set([k for k, v in func_sig.parameters.items()])
    if len(required_params - set(action_args.keys())) > 0:
        raise Exception("requiered params are missing")
    if len(set(action_args.keys()) - all_params) > 0:
        raise Exception("some parameters are unknown")
        
    return True

def verify_actions(actions):
    for action in actions:
        func_name = action.action
        func_args = action.args
        if not func_name in all_actions:
            raise Exception("unknown action %s" % func_name)
        else:
            func_sig = all_actions[func_name][1]
            if not verify_action_args(action.args, func_sig):
                raise Exeption("action args not compatilble for %s" % func_name)

def print_known_actions(out_fd):
    #ipdb.set_trace()
    action_descriptions = []
    for func_name in all_actions.keys():
        func = all_actions[func_name][0]
        action_description = "%s[%s]\n" % (func_name, ",".join(inspect.getargspec(func)[0]))
        action_description += func.__doc__.strip() if func.__doc__ else "\tno description provided"
        action_descriptions.append(action_description)
    if len(action_descriptions) == 0:
        print("no actions", file = out_fd)
    else:
        print("\n".join(action_descriptions), file = out_fd)
    print("Use --help to see help page", file = out_fd)
    sys.exit(23)

def make_call(action):
    func_name = action.action
    func_args = action.args
    func = all_actions[func_name][0]
    return func(**func_args)

def exec_actions(cmdline_args):
    #ipdb.set_trace()
    if len(cmdline_args) == 0:
        print_known_actions(sys.stderr)
        
    opts, actions = parse_cmdline(cmdline_args)
    if opts['help']:
        print_usage("", sys.stderr)

    #ipdb.set_trace()
    if opts['show']:
        show_opts_actions(opts, actions, sys.stderr)
        
    #if opts['debug']:
    #    dbg.enable()
    #else:
    #    dbg.disable()

    if opts['do_dump_config']:
        out_fn = opts['config_dump_fn']
        dump_config(out_fn)
    
    # processing of no_console and no_log
    # will be here

    # processing of actions
    verify_actions(actions)
    for action in actions:
        make_call(action)
