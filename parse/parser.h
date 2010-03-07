#ifndef _PARSER_HPP_
#define _PARSER_HPP_

#include <vector>

enum TokenTag
{
  INVALID = -1,
  ID = 1,
  BOOL_TRUE,
  BOOL_FALSE,
  DEPTH_WRITE_MASK_ALL,
  DEPTH_WRITE_MASK_ZERO,
  DEPTH_ENABLE,
  ASSIGN,
  SEMI_COLON,
  DEPTH_WRITE_MASK,
  DEPTH_STENCIL_STATE,
  L_BRACKET,
  R_BRACKET,
};

struct Token
{
  Token(const TokenTag tag, const int extra = 0) : tag(tag), extra(extra) {}
  TokenTag tag;
  int extra;
};

typedef std::vector<Token> Tokens;
char* make_string(const char* ts, const char* te);


#endif