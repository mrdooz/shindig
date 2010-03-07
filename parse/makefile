DX_DIR = "C:/Program Files (x86)/Microsoft DirectX SDK (February 2010)/Include"
OTHER_DIR = "C:/Program Files/Microsoft SDKs/Windows/v7.0/Include"
MS_DIR = "C:/Program Files (x86)/Microsoft Visual Studio 9.0/VC/include"

CC = "/cygdrive/c/Program Files (x86)/Microsoft Visual Studio 9.0/VC/BIN/cl.exe"

parser : parser_lexer.obj
	$(CC) /EHsc parser_lexer.obj

parser_lexer.obj: parser_lexer.cpp
	$(CC) -c -I$(DX_DIR) -I$(MS_DIR) /EHsc parser_lexer.cpp

parser_lexer.cpp: parser.rl parser.c
	ragel parser.rl -o parser_lexer.cpp

parser.c : parser.y
	lemon parser.y