#ifndef TOKENS_H
#define TOKENS_H

#include "str.h"
#include <stddef.h>

#define SIGCHAR_STRING_BOUNDARY   '\"'
#define SIGCHAR_DECIMAL_POINT     '.'

#define TOKCHAR_COMMA             ','
#define TOKCHAR_EQUALS            '='

#define TOKCHAR_LPAREN            '('
#define TOKCHAR_RPAREN            ')'

#define TOKCHAR_OP_PLUS           '+'
#define TOKCHAR_OP_MINUS          '-'
#define TOKCHAR_OP_MULT           '*'
#define TOKCHAR_OP_DIV            '/'
#define TOKCHAR_OP_POW            '^'

#define KEYWORD_LET               "LET"
#define KEYWORD_PRINT             "PRINT"
#define KEYWORD_INPUT             "INPUT"
#define KEYWORD_GOTO              "GOTO"
#define KEYWORD_GOSUB             "GOSUB"
#define KEYWORD_RETURN            "RETURN"
#define KEYWORD_END               "END"
#define KEYWORD_IF                "IF"
#define KEYWORD_THEN              "THEN"
#define KEYWORD_REM               "REM"

typedef enum {
  tt_Unknown,
  tt_None,
  tt_Identifier,
  tt_Float,
  tt_Long,
  tt_Comma,
  tt_Dot,
  tt_String,
  tt_LParen,
  tt_RParen,
  tt_Operator,
  tt_Keyword,
  tt_Equals,
} TokenType;

typedef enum {
  to_None,
  to_Add,
  to_Sub,
  to_Mult,
  to_Div,
  to_Pow,
} TokenOperator;

typedef enum {
  tk_None,
  tk_Let,
  tk_Print,
  tk_Input,
  tk_Goto,
  tk_Gosub,
  tk_Return,
  tk_End,
  tk_If,
  tk_Then,
  tk_Remark,
} TokenKeyword;

typedef struct {
  TokenType type;
  union {
    TokenOperator operator;
    TokenKeyword keyword;
    float _float;
    long _long;
    Str string;
  };
} Token;


void tok_free(Token* token);
char* tok_get_type_cstr(TokenType type);
void tok_print(Token* token);
TokenKeyword tok_get_keyword(const char* cstr, size_t len);
char* tok_tk_get_cstr(TokenKeyword tk);

#endif
