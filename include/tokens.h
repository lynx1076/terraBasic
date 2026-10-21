#ifndef TOKENS_H
#define TOKENS_H

#include "str.h"
#include <stddef.h>

#define SIGCHAR_STRING_BOUNDARY   '\"'
#define SIGCHAR_DECIMAL_POINT     '.'

#define TOKCHAR_COMMA             ','

#define TOKCHAR_LPAREN            '('
#define TOKCHAR_RPAREN            ')'

#define TOKCHAR_OP_PLUS           '+'
#define TOKCHAR_OP_MINUS          '-'
#define TOKCHAR_OP_MULT           '*'
#define TOKCHAR_OP_DIV            '/'
#define TOKCHAR_OP_POW            '^'


typedef enum {
  tt_Unknown,
  tt_None,
  tt_Identifier,
  tt_Number,
  tt_Comma,
  tt_Dot,
  tt_String,
  tt_LParen,
  tt_RParen,
  tt_Operator,
} TokenType;

typedef struct {
  TokenType type;
  Str string;
} Token;


int tok_init(Token* token, TokenType type, const char* text, size_t length);
void tok_free(Token* token);
char* tok_get_type_cstr(TokenType type);
void tok_print(Token* token);

#endif
