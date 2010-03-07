
#line 1 "d:\\projects\\shindig\\parse\\parser.rl"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <windows.h>
#include <memory.h>
#include <d3d11.h>
#include "parser.h"

// HLSL RasterizerState parser


#line 2 "d:\\projects\\shindig\\parse\\parser_lexer.cpp"
static const char _rasta_man_actions[] = {
	0, 1, 0, 1, 1, 1, 11, 1, 
	12, 1, 13, 1, 14, 1, 15, 1, 
	16, 1, 17, 2, 2, 3, 2, 2, 
	4, 2, 2, 5, 2, 2, 6, 2, 
	2, 7, 2, 2, 8, 2, 2, 9, 
	2, 2, 10
};

static const short _rasta_man_key_offsets[] = {
	0, 0, 20, 23, 31, 37, 45, 52, 
	59, 66, 73, 82, 89, 96, 103, 110, 
	117, 124, 131, 138, 145, 152, 159, 166, 
	173, 180, 187, 194, 201, 208, 215, 222, 
	229, 236, 243, 250, 258, 266, 274, 282, 
	290, 298, 306, 314, 322
};

static const char _rasta_man_trans_keys[] = {
	32, 59, 61, 65, 68, 70, 84, 90, 
	97, 102, 116, 122, 123, 125, 9, 13, 
	66, 89, 98, 121, 32, 9, 13, 76, 
	108, 48, 57, 65, 90, 97, 122, 48, 
	57, 65, 90, 97, 122, 76, 108, 48, 
	57, 65, 90, 97, 122, 101, 48, 57, 
	65, 90, 97, 122, 112, 48, 57, 65, 
	90, 97, 122, 116, 48, 57, 65, 90, 
	97, 122, 104, 48, 57, 65, 90, 97, 
	122, 69, 83, 87, 48, 57, 65, 90, 
	97, 122, 110, 48, 57, 65, 90, 97, 
	122, 97, 48, 57, 65, 90, 98, 122, 
	98, 48, 57, 65, 90, 97, 122, 108, 
	48, 57, 65, 90, 97, 122, 101, 48, 
	57, 65, 90, 97, 122, 116, 48, 57, 
	65, 90, 97, 122, 101, 48, 57, 65, 
	90, 97, 122, 110, 48, 57, 65, 90, 
	97, 122, 99, 48, 57, 65, 90, 97, 
	122, 105, 48, 57, 65, 90, 97, 122, 
	108, 48, 57, 65, 90, 97, 122, 83, 
	48, 57, 65, 90, 97, 122, 116, 48, 
	57, 65, 90, 97, 122, 97, 48, 57, 
	65, 90, 98, 122, 116, 48, 57, 65, 
	90, 97, 122, 101, 48, 57, 65, 90, 
	97, 122, 114, 48, 57, 65, 90, 97, 
	122, 105, 48, 57, 65, 90, 97, 122, 
	116, 48, 57, 65, 90, 97, 122, 101, 
	48, 57, 65, 90, 97, 122, 77, 48, 
	57, 65, 90, 97, 122, 97, 48, 57, 
	65, 90, 98, 122, 115, 48, 57, 65, 
	90, 97, 122, 107, 48, 57, 65, 90, 
	97, 122, 65, 97, 48, 57, 66, 90, 
	98, 122, 76, 108, 48, 57, 65, 90, 
	97, 122, 83, 115, 48, 57, 65, 90, 
	97, 122, 69, 101, 48, 57, 65, 90, 
	97, 122, 82, 114, 48, 57, 65, 90, 
	97, 122, 85, 117, 48, 57, 65, 90, 
	97, 122, 69, 101, 48, 57, 65, 90, 
	97, 122, 69, 101, 48, 57, 65, 90, 
	97, 122, 82, 114, 48, 57, 65, 90, 
	97, 122, 79, 111, 48, 57, 65, 90, 
	97, 122, 0
};

static const char _rasta_man_single_lengths[] = {
	0, 14, 1, 2, 0, 2, 1, 1, 
	1, 1, 3, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 2, 2, 2, 2, 2, 
	2, 2, 2, 2, 2
};

static const char _rasta_man_range_lengths[] = {
	0, 3, 1, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3, 3, 3, 3, 
	3, 3, 3, 3, 3
};

static const short _rasta_man_index_offsets[] = {
	0, 0, 18, 21, 27, 31, 37, 42, 
	47, 52, 57, 64, 69, 74, 79, 84, 
	89, 94, 99, 104, 109, 114, 119, 124, 
	129, 134, 139, 144, 149, 154, 159, 164, 
	169, 174, 179, 184, 190, 196, 202, 208, 
	214, 220, 226, 232, 238
};

static const char _rasta_man_indicies[] = {
	0, 2, 3, 4, 6, 7, 8, 9, 
	4, 7, 8, 9, 10, 11, 0, 5, 
	5, 1, 0, 0, 12, 14, 14, 5, 
	5, 5, 13, 5, 5, 5, 15, 16, 
	16, 5, 5, 5, 13, 17, 5, 5, 
	5, 13, 18, 5, 5, 5, 13, 19, 
	5, 5, 5, 13, 20, 5, 5, 5, 
	13, 21, 22, 23, 5, 5, 5, 13, 
	24, 5, 5, 5, 13, 25, 5, 5, 
	5, 13, 26, 5, 5, 5, 13, 27, 
	5, 5, 5, 13, 28, 5, 5, 5, 
	13, 29, 5, 5, 5, 13, 30, 5, 
	5, 5, 13, 31, 5, 5, 5, 13, 
	32, 5, 5, 5, 13, 33, 5, 5, 
	5, 13, 34, 5, 5, 5, 13, 35, 
	5, 5, 5, 13, 36, 5, 5, 5, 
	13, 37, 5, 5, 5, 13, 38, 5, 
	5, 5, 13, 39, 5, 5, 5, 13, 
	40, 5, 5, 5, 13, 41, 5, 5, 
	5, 13, 42, 5, 5, 5, 13, 43, 
	5, 5, 5, 13, 44, 5, 5, 5, 
	13, 45, 5, 5, 5, 13, 46, 5, 
	5, 5, 13, 47, 5, 5, 5, 13, 
	48, 48, 5, 5, 5, 13, 49, 49, 
	5, 5, 5, 13, 50, 50, 5, 5, 
	5, 13, 51, 51, 5, 5, 5, 13, 
	52, 52, 5, 5, 5, 13, 53, 53, 
	5, 5, 5, 13, 54, 54, 5, 5, 
	5, 13, 55, 55, 5, 5, 5, 13, 
	56, 56, 5, 5, 5, 13, 57, 57, 
	5, 5, 5, 13, 0
};

static const char _rasta_man_trans_targs[] = {
	2, 0, 1, 1, 3, 4, 6, 35, 
	39, 42, 1, 1, 1, 1, 5, 1, 
	4, 7, 8, 9, 10, 11, 16, 27, 
	12, 13, 14, 15, 4, 17, 18, 19, 
	20, 21, 22, 23, 24, 25, 26, 4, 
	28, 29, 30, 31, 32, 33, 34, 4, 
	36, 37, 38, 4, 40, 41, 4, 43, 
	44, 4
};

static const char _rasta_man_trans_actions[] = {
	0, 0, 9, 11, 0, 40, 0, 0, 
	0, 0, 5, 7, 15, 13, 0, 17, 
	34, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 22, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 19, 
	0, 0, 0, 0, 0, 0, 0, 25, 
	0, 0, 0, 31, 0, 0, 28, 0, 
	0, 37
};

static const char _rasta_man_to_state_actions[] = {
	0, 1, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const char _rasta_man_from_state_actions[] = {
	0, 3, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0
};

static const short _rasta_man_eof_trans[] = {
	0, 0, 13, 14, 16, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14, 14, 14, 14, 
	14, 14, 14, 14, 14
};

static const int rasta_man_start = 1;
static const int rasta_man_first_final = 1;
static const int rasta_man_error = 0;

static const int rasta_man_en_main = 1;


#line 15 "d:\\projects\\shindig\\parse\\parser.rl"


int parse_inner(Tokens& tokens, char* str)
{
  char *p = str, *pe = str + strlen(str);
  int cs, act = 0;
  char *ts, *te, *tm = 0, *eof = 0;  

  
#line 181 "d:\\projects\\shindig\\parse\\parser_lexer.cpp"
	{
	cs = rasta_man_start;
	ts = 0;
	te = 0;
	act = 0;
	}

#line 187 "d:\\projects\\shindig\\parse\\parser_lexer.cpp"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_acts = _rasta_man_actions + _rasta_man_from_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 1:
#line 1 "NONE"
	{ts = p;}
	break;
#line 206 "d:\\projects\\shindig\\parse\\parser_lexer.cpp"
		}
	}

	_keys = _rasta_man_trans_keys + _rasta_man_key_offsets[cs];
	_trans = _rasta_man_index_offsets[cs];

	_klen = _rasta_man_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _rasta_man_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _rasta_man_indicies[_trans];
_eof_trans:
	cs = _rasta_man_trans_targs[_trans];

	if ( _rasta_man_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _rasta_man_actions + _rasta_man_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 2:
#line 1 "NONE"
	{te = p+1;}
	break;
	case 3:
#line 37 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 5;}
	break;
	case 4:
#line 38 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 6;}
	break;
	case 5:
#line 39 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 7;}
	break;
	case 6:
#line 40 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 8;}
	break;
	case 7:
#line 41 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 9;}
	break;
	case 8:
#line 42 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 10;}
	break;
	case 9:
#line 43 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 11;}
	break;
	case 10:
#line 45 "d:\\projects\\shindig\\parse\\parser.rl"
	{act = 12;}
	break;
	case 11:
#line 32 "d:\\projects\\shindig\\parse\\parser.rl"
	{te = p+1;{ tokens.push_back(Token(L_BRACKET, 0)); }}
	break;
	case 12:
#line 33 "d:\\projects\\shindig\\parse\\parser.rl"
	{te = p+1;{ tokens.push_back(Token(R_BRACKET, 0)); }}
	break;
	case 13:
#line 34 "d:\\projects\\shindig\\parse\\parser.rl"
	{te = p+1;{ tokens.push_back(Token(SEMI_COLON, 0)); }}
	break;
	case 14:
#line 35 "d:\\projects\\shindig\\parse\\parser.rl"
	{te = p+1;{ tokens.push_back(Token(ASSIGN, 0)); }}
	break;
	case 15:
#line 45 "d:\\projects\\shindig\\parse\\parser.rl"
	{te = p;p--;{ tokens.push_back(Token(ID, (int)make_string(ts, te))); }}
	break;
	case 16:
#line 46 "d:\\projects\\shindig\\parse\\parser.rl"
	{te = p;p--;{ }}
	break;
	case 17:
#line 1 "NONE"
	{	switch( act ) {
	case 5:
	{{p = ((te))-1;} tokens.push_back(Token(DEPTH_STENCIL_STATE, 0)); }
	break;
	case 6:
	{{p = ((te))-1;} tokens.push_back(Token(DEPTH_ENABLE, 0)); }
	break;
	case 7:
	{{p = ((te))-1;} tokens.push_back(Token(DEPTH_WRITE_MASK, 0)); }
	break;
	case 8:
	{{p = ((te))-1;} tokens.push_back(Token(BOOL_TRUE, 0)); }
	break;
	case 9:
	{{p = ((te))-1;} tokens.push_back(Token(BOOL_FALSE, 0)); }
	break;
	case 10:
	{{p = ((te))-1;} tokens.push_back(Token(DEPTH_WRITE_MASK_ALL, 0)); }
	break;
	case 11:
	{{p = ((te))-1;} tokens.push_back(Token(DEPTH_WRITE_MASK_ZERO, 0)); }
	break;
	case 12:
	{{p = ((te))-1;} tokens.push_back(Token(ID, (int)make_string(ts, te))); }
	break;
	}
	}
	break;
#line 345 "d:\\projects\\shindig\\parse\\parser_lexer.cpp"
		}
	}

_again:
	_acts = _rasta_man_actions + _rasta_man_to_state_actions[cs];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 ) {
		switch ( *_acts++ ) {
	case 0:
#line 1 "NONE"
	{ts = 0;}
	break;
#line 356 "d:\\projects\\shindig\\parse\\parser_lexer.cpp"
		}
	}

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	if ( _rasta_man_eof_trans[cs] > 0 ) {
		_trans = _rasta_man_eof_trans[cs] - 1;
		goto _eof_trans;
	}
	}

	_out: {}
	}

#line 51 "d:\\projects\\shindig\\parse\\parser.rl"

 
  return 1;
}

