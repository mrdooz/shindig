# my first parser generator!
# builds recursive decent parsers

# * = zero or more
# + = one or more

from string import Template
from shaker_maker_templates import *
import re

matcher_prefix = "match_"

#    ::=\s*(?P<rules>[\w\|\s\*\+\[\]]+?)\s* # rules

rule_re = re.compile(r"""
    (?P<name>\w+)\s*  # non-terminal
    (?:\((?P<params>.*)\))?\s* # parameters
    (?:\s*:\s*(?P<type>.*))?\s* # type
    ::=\s*(?P<rules>.+?)\s* # rules
    (?:@pre\s*{\s*(?P<pre_code>.*?)\s*}\s*)* #pre code
    (?:@pass\s*{\s*(?P<pass_code>.*?)\s*}\s*)* #pass code
    (?:@fail\s*{\s*(?P<fail_code>.*?)\s*}\s*)* #fail code
    \.$""", re.VERBOSE)

class Rule(object):
    def __init__(self, rule_str):
        def process_repeat(a):
            self.repeat = a[-1] if a[-1] in "+*" else None
            return a[:-1] if self.repeat else a
        def process_optional(a):
            def is_optional(a): return a[0] == "[" and a[-1] == "]"
            self.options = [x.strip() for x in a[1:-1].split("|")] if is_optional(a) else None
        
        self.match = rule_re.match(rule_str)
        if not self.match:
            raise Exception, "Invalid string pass to Rule: " + rule_str
        self.rule_str = rule_str[:-1]
        s = self.rule_str.split("::=")
        self.lhs = self.match.group("name").strip()
        self.rhs = self.match.group("rules").strip()
            
        self.params = "" if not self.match.group("params") else self.match.group("params")
        self.p_type = "" if not self.match.group("type") else self.match.group("type")
        self.pre_code = "" if not self.match.group("pre_code") else self.match.group("pre_code")
        self.pass_code = "" if not self.match.group("pass_code") else self.match.group("pass_code")
        self.fail_code = "" if not self.match.group("fail_code") else self.match.group("fail_code")

        # process repeats, options and compounds
        a = process_repeat(self.rhs)
        process_optional(a)
        self.compound = [] if self.options else a.split()

    def complex(self):
        return self.compound or self.options
            
    def __str__(self):
        return self.rule_str

def rule_gen(lines):
    # Generator that yields Rules from the input grammar
    idx = 0
    end = len(lines)
    while idx < end:
        cur = ""
        while not cur.endswith("."):
            s = lines[idx].strip()
            idx = idx + 1
            if idx > end: raise Exception("No trailing . found")
            if not s.startswith("#"):
                cur += s
        # cur is the whole line up to the trailing "."
        yield Rule(cur)
        
def is_terminal(symbol):
    return re.match(r"^[A-Z0-9_]+$", symbol)

def is_non_terminal(symbol):
    return not is_terminal(symbol)

def matcher_name(str):
    if is_terminal(str):
        return matcher_prefix + str.lower() + "_t"
    else:
        return matcher_prefix + str

def create_option_matcher(r, is_inner):
    # an option matcher consists of an outer-match that contains
    # a sequence of "if xxx return true;" statements
    
    # create inner matchers
    inner = [optional_matcher_code_inner.substitute({
        "MATCH" : matcher_name(x),
        "PARAMS" : "",
        "PASS_CODE" : r.pass_code }) 
             for x in r.options]

    # create outer matcher
    name = matcher_prefix + r.lhs
    
    # if this matcher is an inner matcher, append a suffix
    if is_inner: 
        name += "_inner"

    return optional_matcher_code.substitute(
        {"RULE" : r, 
         "NAME" : name, 
         "PARAMS" : r.params,
         "TYPE" : r.p_type,
         "INNER" : "\n".join(inner),
         "PRE_CODE" : r.pre_code,
         "FAIL_CODE" : r.fail_code
         })

def create_repeat_matcher(r):
    # create matcher for * and +
    
    if len(r.compound) > 1:
        print r
        raise "We can only handle 1 rule in the repeat"

    # we assume that the rule we're matching is complex, which means
    # that it's going to be called lhs + _inner.
    return repeat_matcher_code.substitute(
        {"RULE" : r, 
         "NAME" : matcher_prefix + r.lhs, 
         "PARAMS" : r.params,
         "TYPE" : r.p_type,
         "MATCH" : matcher_prefix + r.lhs + "_inner",
         "EQ" : { "*" : ">=", "+" : ">" }[r.repeat],
         "PRE_CODE" : r.pre_code,
         "PASS_CODE" : r.pass_code,
         "FAIL_CODE" : r.fail_code
         })

def create_compound_matcher(r, is_inner):
    # a compound matcher is a matcher that matches a sequence of rules
    m = [compound_matcher_code_inner.substitute({
        "MATCH" : matcher_name(x),
        "PARAMS" : "",
        "FAIL_CODE" : r.fail_code})
         for x in r.compound]

    name = matcher_prefix + r.lhs
    if is_inner: 
        name += "_inner"
        
    return compound_matcher_code.substitute(
        {"NAME" : name,
         "PARAMS" : r.params,
         "TYPE" : r.p_type,
         "RULE" : str(r),
         "INNER" : "\n".join(m),
         "PRE_CODE" : r.pre_code,
         "PASS_CODE" : r.pass_code
         })

def create_matchers(rules):
    def collect_symbols(rules):
    # collect terminals and non-terminals
        t = set()
        n = set()
        for r in rules:
            n.add(r.lhs)
            for i in (r.options if r.options else r.compound):
                if is_terminal(i):
                    t.add(i)
                else:
                    n.add(i)
        return t, n
    matchers = []
    terminals, non_terminals = collect_symbols(rules)
    tokens = []
    terminal_matchers = []
    
    # create the terminal matchers
    for t in terminals:
        tokens.append(t)
        terminal_matchers.append(terminal_matcher_code.substitute(
            {"NAME": matcher_name(t),
             "SYMBOL" : t}))

    matchers.append(parser_code_prolog.substitute( { "PARSER_NAME" : "Parser"}))
    matchers.append("\n".join(terminal_matchers))

    for r in rules:
        is_inner = r.repeat
        if r.options:
            matchers.append(create_option_matcher(r, is_inner))
        if r.compound:
            matchers.append(create_compound_matcher(r, is_inner))
        if r.repeat:
            matchers.append(create_repeat_matcher(r))

    matchers.append(parser_code_epilog.substitute({}))

    return matchers, tokens

rules = [r for r in rule_gen(file("gram.tjong").readlines())]
matchers, tokens = create_matchers(rules)
open("mr_parser.hpp", "wt").write("\n".join(matchers))
open("mr_parser_symbols.hpp", "wt").write(parser_header_code.substitute(
        {"TOKENS" : ",\n".join(tokens)}))

print "done"
