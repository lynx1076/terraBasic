#ifndef TOKENS_H
#define TOKENS_H

#include "str.h"
#include <stddef.h>

#define SIGCHAR_STRING_BOUNDARY   '\"'
#define SIGCHAR_DECIMAL_POINT     '.'

#define TOK_OP_COMMA          ","

#define TOK_OP_LPAREN         "("
#define TOK_OP_RPAREN         ")"

#define TOK_OP_PLUS           "+"
#define TOK_OP_MINUS          "-"
#define TOK_OP_MULT           "*"
#define TOK_OP_DIV            "/"
#define TOK_OP_POW            "^"

#define TOK_OP_CMP_L          "<"
#define TOK_OP_CMP_G          ">"
#define TOK_OP_CMP_LT         "<="
#define TOK_OP_CMP_GT         ">="
#define TOK_OP_CMP_EQ         "="
#define TOK_OP_CMP_NEQ        "<>"

#define TOK_OPS \
  X(TOK_OP_COMMA, to_Comma) \
  X(TOK_OP_LPAREN, to_LParen) \
  X(TOK_OP_RPAREN, to_RParen) \
  X(TOK_OP_PLUS, to_Plus) \
  X(TOK_OP_MINUS, to_Minus) \
  X(TOK_OP_MULT, to_Mult) \
  X(TOK_OP_DIV, to_Div) \
  X(TOK_OP_POW, to_Pow) \
  X(TOK_OP_CMP_L, to_CmpL) \
  X(TOK_OP_CMP_G, to_CmpG) \
  X(TOK_OP_CMP_LT, to_CmpLT) \
  X(TOK_OP_CMP_GT, to_CmpGT) \
  X(TOK_OP_CMP_EQ, to_CmpEq) \
  X(TOK_OP_CMP_NEQ, to_CmpNEq) \

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
  tt_Function,
  tt_Identifier,
  tt_Float,
  tt_Long,
  tt_Comma,
  tt_Dot,
  tt_String,
  tt_Operator,
  tt_Keyword,
} TokenType;

typedef enum {
  to_None,
#define X(TOK_OP, TOK_ENUM) TOK_ENUM,
  TOK_OPS
#undef X
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
  size_t argCount;
  Str name;
} TokenFunction;

typedef struct {
  TokenType type;
  union {
    TokenFunction func;
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
const char* tok_get_operator_cstr(TokenOperator op);
TokenKeyword tok_get_keyword(const char* cstr, size_t len);
char* tok_tk_get_cstr(TokenKeyword tk);
int tok_get_operator_precedence(TokenOperator op);

#endif
