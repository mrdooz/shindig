#code templates for shaker_maker

from string import Template

parser_header_code = Template("""
#ifndef _PARSER_TOKENS_HPP_
#define _PARSER_TOKENS_HPP_
#include <stack>
#include <vector>

enum TokenTag
{
    INVALID = -1,
    $TOKENS
};

struct Token
{
  Token(const TokenTag tag, const int extra = 0) : tag(tag), extra(extra) {}
  Token(const TokenTag tag, const float extra = 0) : tag(tag), extra_float(extra) {}
  TokenTag tag;
union {
  int extra;
  const char* _str;
  float extra_float;
};
};
typedef std::vector<Token> Tokens;

#include <map>
#include <string>

struct DD3D11_BLEND_DESC : public CD3D11_BLEND_DESC { DD3D11_BLEND_DESC() : CD3D11_BLEND_DESC(D3D11_DEFAULT) {} };
struct DD3D11_RASTERIZER_DESC : public CD3D11_RASTERIZER_DESC { DD3D11_RASTERIZER_DESC() : CD3D11_RASTERIZER_DESC(D3D11_DEFAULT) {} };
struct DD3D11_SAMPLER_DESC : public CD3D11_SAMPLER_DESC { DD3D11_SAMPLER_DESC() : CD3D11_SAMPLER_DESC(D3D11_DEFAULT) {Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; } };

typedef std::map< std::string, DD3D11_BLEND_DESC> BlendDescs;
typedef std::map< std::string, DD3D11_RASTERIZER_DESC> RasterizerDescs;
typedef std::map< std::string, DD3D11_SAMPLER_DESC> SamplerDescs;

struct BigState
{
  BlendDescs _blend_descs;
  RasterizerDescs _rasterizer_descs;
  SamplerDescs _sampler_descs;
};
#endif
""")

optional_matcher_code = Template("""
bool $NAME($TYPE $PARAMS)
{
    // $RULE
    $PRE_CODE
    $INNER
    $FAIL_CODE
    return false;
}
""")

optional_matcher_code_inner = Template("""
    push_idx();
    if ($MATCH($PARAMS) ) {
        $PASS_CODE
        return true;
    }
    pop_idx();
""")

compound_matcher_code = Template("""
bool $NAME($TYPE $PARAMS)
{
    // $RULE
    $BACK_REFS
    $PRE_CODE
    $INNER
    $PASS_CODE
    return true;
}
""")

compound_matcher_code_inner = Template("""
    $BACK_REFS
    if (!$MATCH($PARAMS)) {
        $FAIL_CODE
        return false;
    }
""")

repeat_matcher_code = Template("""
bool $NAME($TYPE $PARAMS)
{
    // $RULE
    $PRE_CODE
    int count = 0;
    while ($MATCH($PARAMS)) {
        ++count;
    }
    if (count $EQ 0) {
        $PASS_CODE
        return true;
    } else {
        $FAIL_CODE
        return false;
    }
}
""")

terminal_matcher_code = Template("""
bool $NAME()
{
\treturn match($SYMBOL);
}
""")

parser_code_prolog = Template("""
#ifndef _MR_PARSER_HPP_
#define _MR_PARSER_HPP_
#include $SYMBOLS_FILE
class $PARSER_NAME
{
public:
\t$PARSER_NAME(const Tokens& tokens) : _tokens(tokens), _idx(0) {}

\tbool run()
\t{
\t\treturn match_start();
\t}
$CLASS_DATA
private:
""")

parser_code_epilog = Template("""
\tvoid push_idx()
\t{
\t\t_idx_stack.push(_idx);
\t}

\tvoid pop_idx()
\t{
\t\t_idx = _idx_stack.top();		
\t\t_idx_stack.pop();
\t}

\tbool match(const TokenTag cur)
\t{
\t\tif (cur == lookahead()) {
\t\t\t_idx++;
\t\t\treturn true;
\t\t}
\treturn false;
\t}
	
	TokenTag lookahead()
	{
		if (_idx >= (int)_tokens.size()) {
			return INVALID;
		}
		return _tokens[_idx].tag;
	}

\ttypedef std::stack<int> IdxStack;
\tIdxStack _idx_stack;
\tint _idx;
\tTokens _tokens;

};
#endif
""")

