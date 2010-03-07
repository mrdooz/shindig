#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <memory.h>
#include <d3d11.h>
#include "parser.h"

// HLSL RasterizerState parser

%%{
  machine rasta_man;
  write data;
}%%

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
	"{" => { tokens.push_back(Token(L_BRACKET, 0)); };
	"}" => { tokens.push_back(Token(R_BRACKET, 0)); };
	";" => { tokens.push_back(Token(SEMI_COLON, 0)); };
	"=" => { tokens.push_back(Token(ASSIGN, 0)); };
	
	"DepthStencilState" => { tokens.push_back(Token(DEPTH_STENCIL_STATE, 0)); };
	"DepthEnable" => { tokens.push_back(Token(DEPTH_ENABLE, 0)); };
	"DepthWriteMask" => { tokens.push_back(Token(DEPTH_WRITE_MASK, 0)); };
	"DepthFunc" => { tokens.push_back(DEPTH_FUNC, 0)); };
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

