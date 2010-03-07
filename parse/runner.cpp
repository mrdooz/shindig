#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <memory.h>
#include <d3d11.h>
#include <stdint.h>
#include "parser.h"
//#include "parser_lexer.cpp"

bool load_file(uint8_t*& buf, uint32_t& len, const char* filename, const bool zero_terminate)
{
  const HANDLE file_handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, 
    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (file_handle == INVALID_HANDLE_VALUE) {
    return false;
  }

  const uint32_t file_size = GetFileSize(file_handle, NULL);
  len = file_size + (zero_terminate ? 1 : 0);
  buf = new BYTE[len];
  DWORD bytes_read = 0;
  if (!ReadFile(file_handle, buf, file_size, &bytes_read, NULL) || bytes_read != file_size) {
    return false;
  }

  CloseHandle(file_handle);
  if (zero_terminate) {
    buf[file_size] = 0;
  }
  return true;
}


// HLSL RasterizerState parser

extern int parse_inner(Tokens& tokens, char* str);
char* make_string(const char* ts, const char* te)
{
	const int len = te - ts;
	char* buf = new char[len+1];
	memcpy(buf, ts, len);
	buf[len] = 0;
	return buf;
}

/*

	BNF-ish syntax
	
	start ::= "DepthStencilState" id "{" var* "}" ";"
	var ::= depth_enable | depth_write_mask ";"
	depth_enable ::= DEPTH_ENABLE "=" bool ";"
	bool ::= "true" | "false";

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
	
  bool in(const TokenTag tag, TokenTag* vars, const int num)
  {
    for (int i = 0; i < num; ++i) {
      if (vars[i] == tag) {
        return true;
      }
    }
    return false;
  }

#define ELEMS_IN_ARRAY(x) (sizeof(x) / sizeof((x)[0]))
#define IN_ARRAY(x, y) in((x), (y), ELEMS_IN_ARRAY(y))

	void dss_var()
	{
    TokenTag vars[] = { DEPTH_ENABLE, DEPTH_WRITE_MASK };
    while (IN_ARRAY(lookahead(), vars)) {
      switch(lookahead())
      {
      case DEPTH_ENABLE:
        depth_enable();
        break;
      }
    }
	}
	
  void depth_write_mask()
  {
    match(DEPTH_WRITE_MASK);
    match(ASSIGN);

    match(SEMI_COLON);
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
	
	void match(const TokenTag cur)
	{
		// compares the look-ahead with the token, and croak if different
		if (cur == lookahead()) {
			_idx++;
			return;
		}
		printf("fail\n");
		assert(false);
	}
	
	TokenTag lookahead()
	{
		if (_idx >= (int)_tokens.size()) {
			return INVALID;
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

int main(int argc, char** argv)
{

  uint8_t* buf;
  uint32_t len;
  if (!load_file(buf, len, argv[1], true)) {
    return 1;
  }

	Tokens tokens;
  parse_inner(tokens, (char*)buf);
	
	Parser p(tokens);
	p.run();

  delete [] buf;
	
  return 0;
}
