# my first parser generator!
# builds recursive decent parsers
# magnus osterlind, 2010

# * = zero or more
# + = one or more

from string import Template
from shaker_maker_templates import *
from itertools import izip_longest
from optparse import OptionParser
import re

matcher_prefix = "match_"
parser_name = ""
symbols_file = ""

class_data = []

rule_re = re.compile(r"""
    (?P<name>\w+)\s*  # non-terminal
    (?:\((?P<params>.*)\))?\s* # parameters
    (?:\s*:\s*(?P<type>.*))?\s* # type
    ::=\s*(?P<rules>.+?)\s* # rules
    (?:@pre\s*{\s*(?P<pre_code>.*?)\s*}\s*)* #pre code
    (?:@pass\s*{\s*(?P<pass_code>.*?)\s*}\s*)* #pass code
    (?:@fail\s*{\s*(?P<fail_code>.*?)\s*}\s*)* #fail code
    \.$""", re.VERBOSE)

def make_func_pair(str):
    # given a string of func[(xx)], returns a pair (func, xx)
    s = str.find("(")
    e = str.find(")")
    if s != -1 and e != -e:
        f = str[:s]
        p = str[s+1:e]
    else:
        f = str
        p = None
    return (f,p)

def grouper(n, iterable, fillvalue=None):
    "grouper(3, 'ABCDEFG', 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return izip_longest(fillvalue=fillvalue, *args)

def get_between(str, c):
    # return a list of substrings in str that appear between c
    if not str: return []
    m = [x for x in xrange(len(str)) if str[x] == c]
    r = []
    for (a, b) in grouper(2, m):
        r.append(str[a+1:b])
    return r

class Rule(object):
    def __init__(self, rule_str):
        def process_repeat(a):
            self.repeat = a[-1] if a[-1] in "+*" else None
            return a[:-1] if self.repeat else a
        def process_optional(a):
            def is_optional(a): return a[0] == "[" and a[-1] == "]"
            if is_optional(a):
                xs = [x.strip() for x in a[1:-1].split("|")]
                self.options = [make_func_pair(x) for x in xs]
            else:
                self.options = None
        def add_backref(id):
            if not id:
                return
            # check that the back reference exists
            if next((f for f,p in self.compound if f == id), None):
                self.back_refs.append(id)
            else:
                raise Exception, "Invalid back reference: " + id
            
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
        self.back_refs = []
        if not self.options:
            # check if any compounds contain back references
            self.compound = [make_func_pair(x) for x in a.split()]
            for f,p in self.compound:
                for b in get_between(p, "$"):
                    add_backref(b)
        else:
            self.compound = []
            
        # check for back-references in the code blocks
        if self.pass_code:
            for b in get_between(self.pass_code, "$"):
                add_backref(b)

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
            if s.startswith("#"):
                # skip comments
                pass
            elif s.startswith("%class%"):
                # copy class members
                class_data.append(s[len("%class%"):])
            else:
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

def replace_backref(p):
    # replace all occurences of $ID$ with _tokens[backrefs[ID])
    if not p: return ""
    m = [x for x in xrange(len(p)) if p[x] == "$"]
    if len(m) == 0: return p
    refs = []
    for (a, b) in grouper(2, m):
        refs.append(p[a+1:b])
    for r in refs:
        p = p.replace("$" + r + "$", "_tokens[back_refs[%s]]" % r)
    return p

def create_option_matcher(r, is_inner):
    # an option matcher consists of an outer-match that contains
    # a sequence of "if xxx return true;" statements
    
    # create inner matchers
    inner = [optional_matcher_code_inner.substitute({
        "MATCH" : matcher_name(f),
        "PARAMS" : p if p else "",
        "PASS_CODE" : r.pass_code }) 
             for f, p in r.options]

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
    use_backrefs = len(r.back_refs) > 0
    m = []
    # a compound matcher is a matcher that matches a sequence of rules
    for f, p in r.compound:
        b = ""
        if use_backrefs and is_terminal(f):
            b = "back_refs[%s] = _idx;" % f
        pp = replace_backref(p)
        cur = compound_matcher_code_inner.substitute({
        "MATCH" : matcher_name(f),
        "BACK_REFS" : b,
        "PARAMS" : pp,
        "FAIL_CODE" : r.fail_code})
        m.append(cur)
        
    name = matcher_prefix + r.lhs
    if is_inner: 
        name += "_inner"

    back_refs = ""
    if len(r.back_refs) > 0:
        back_refs = "std::map<TokenTag, int> back_refs;"        
        
    return compound_matcher_code.substitute(
        {"NAME" : name,
         "BACK_REFS" : back_refs,
         "PARAMS" : r.params,
         "TYPE" : r.p_type,
         "RULE" : str(r),
         "INNER" : "\n".join(m),
         "PRE_CODE" : r.pre_code,
         "PASS_CODE" : replace_backref(r.pass_code)
         })

def create_matchers(rules):
    def collect_symbols(rules):
    # collect terminals and non-terminals
        t = set()
        n = set()
        for r in rules:
            n.add(r.lhs)
            for f,p in (r.options if r.options else r.compound):
                if is_terminal(f):
                    t.add(f)
                else:
                    n.add(f)
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

    matchers.append(parser_code_prolog.substitute( { 
        "PARSER_NAME" : parser_name,
        "SYMBOLS_FILE" : '"%s"' % symbols_file,
        "CLASS_DATA" : "\n".join(class_data)
    }))
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

parser = OptionParser()
parser.add_option("-o", "--out_file", dest="outfile", default="state_parser.hpp")
parser.add_option("-s", "--out_sybols", dest="outfile_symbols", default="state_parser_symbols.hpp")
parser.add_option("-g", "--grammar_file", dest="grammar_file", default="states.sm")
parser.add_option("-p", "--parser_name", dest="parser_name", default="StateParser")

options, args = parser.parse_args()
parser_name = options.parser_name
symbols_file = options.outfile_symbols

rules = [r for r in rule_gen(file(options.grammar_file).readlines())]
matchers, tokens = create_matchers(rules)
open(options.outfile, "wt").write("\n".join(matchers))
open(options.outfile_symbols, "wt").write(parser_header_code.substitute(
        {"TOKENS" : ",\n".join(tokens)}))

print "done"
