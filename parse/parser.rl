#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <memory.h>
#include <d3d11.h>
#include "parser.h"
//#include "parser.c"

// HLSL RasterizerState parser

%%{
  machine rasta_man;
  write data;
}%%

#include <vector>

struct Token
{
	Token(const int tag, const int extra = 0) : tag(tag), extra(extra) {}
	int tag;
	int extra;
};

typedef std::vector<Token> Tokens;

char* make_string(const char* ts, const char* te)
{
	const int len = te - ts;
	char* buf = new char[len+1];
	memcpy(buf, ts, len);
	buf[len] = 0;
	return buf;
}

int parse_inner(Tokens& tokens, char* str)
{
  char *p = str, *pe = str + strlen(str);
  int cs, act = 0;
  char *ts, *te, *tm = 0, *eof = 0;  

  %%{
  id = alpha+alnum*;
  true_ic = [Tt][Rr][Uu][Ee];
  false_ic = [Ff][Aa][Ll][Ss][Ee];
  
  depth_write_mask_zero_ic = [Zz][Ee][Rr][Oo];
  depth_write_mask_all_ic = [Aa][Ll][Ll];

  main :=  |*
	"DepthStencilState" => { tokens.push_back(Token(DEPTH_STENCIL_STATE, 0)); };
	"{" => { tokens.push_back(Token(L_BRACKET, 0)); };
	"}" => { tokens.push_back(Token(R_BRACKET, 0)); };
	";" => { tokens.push_back(Token(SEMI_COLON, 0)); };
	"DepthEnable" => { tokens.push_back(Token(DEPTH_ENABLE, 0)); };
	"DepthWriteMask" => { tokens.push_back(Token(DEPTH_WRITE_MASK, 0)); };
	"=" => { tokens.push_back(Token(ASSIGN, 0)); };
    true_ic => { tokens.push_back(Token(BOOL_TRUE, 0)); };
    false_ic => { tokens.push_back(Token(BOOL_FALSE, 0)); };
	depth_write_mask_all_ic => { tokens.push_back(Token(DEPTH_WRITE_MASK_ALL, 0)); };
	depth_write_mask_zero_ic => { tokens.push_back(Token(DEPTH_WRITE_MASK_ZERO, 0)); };
	id => { tokens.push_back(Token(ID, (int)make_string(ts, te))); };
	space+ => { };
    *|;

    write init;
    write exec;
  }%%
 
  return 1;
}

/*

	BNF-ish syntax
	
	start ::= DEPTH_STENCIL_STATE id L_BRACKET var* R_BRACKET SEMI_COLON
	var ::= depth_enable | depth_write_mask SEMI_COLON
	depth_enable ::= DEPTH_ENABLE EQ bool SEMI_COLOR
	bool ::= BOOL_TRUE | BOOL_FALSE;

*/

// recursive decent parser
struct Parser
{
	Parser(const Tokens& tokens) : _tokens(tokens), _idx(0) {}
	
	void run()
	{
		start();
		printf("pass\n");
	}
	
	void dss_var()
	{
		switch(lookahead())
		{
			case DEPTH_ENABLE:
			depth_enable();
			break;
		}
	}
	
	void depth_enable()
	{
		match(DEPTH_ENABLE);
		match(ASSIGN);
		bool value = do_bool();
		match(SEMI_COLON);
	}
	
	bool do_bool()
	{
		switch(lookahead())
		{
			case BOOL_TRUE:
				match(BOOL_TRUE);
				return true;
			case BOOL_FALSE:
				match(BOOL_FALSE);
				return false;
		}
		return false;
	}
	
	void start()
	{
		match(DEPTH_STENCIL_STATE);
		const std::string str(id());
		match(L_BRACKET);
		dss_var();
		match(R_BRACKET);
		match(SEMI_COLON);
	}
	
	std::string id()
	{
		const std::string str((const char*)cur().extra);
		match(ID);
		return str;
	}
	
	void match(const int cur)
	{
		// compares the look-ahead with the token, and croak if different
		if (cur == lookahead()) {
			_idx++;
			return;
		}
		printf("fail\n");
		assert(false);
	}
	
	int lookahead()
	{
		if (_idx >= _tokens.size()) {
			return -1;
		}
		return _tokens[_idx].tag;
	}
	
	const Token& cur()
	{
		return _tokens[_idx];
	}
	
	int _idx;
	Tokens _tokens;
};

#define BUFSIZE 1024

int main()
{
  char buf[BUFSIZE];
	Tokens tokens;
  while ( fgets( buf, sizeof(buf), stdin ) != 0 ) {
    parse_inner(tokens, buf);
  }
	
	Parser p(tokens);
	p.run();
	
  return 0;
}
