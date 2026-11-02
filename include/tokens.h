#ifndef TOKENS_H
#define TOKENS_H

#include "str.h"
#include <stddef.h>

#define SIGCHAR_STRING_BOUNDARY   '\"'
#define SIGCHAR_DECIMAL_POINT     '.'

#define TOK_OP_COMMA              ","

#define TOK_OP_LPAREN             "("
#define TOK_OP_RPAREN             ")"

#define TOK_OP_PLUS               "+"
#define TOK_OP_MINUS              "-"
#define TOK_OP_MULT               "*"
#define TOK_OP_DIV                "/"
#define TOK_OP_POW                "^"

#define TOK_OP_LT                 "<"
#define TOK_OP_GT                 ">"
#define TOK_OP_LE                 "<="
#define TOK_OP_GE                 ">="
#define TOK_OP_EQ                 "="
#define TOK_OP_NEQ                "<>"

#define TOK_OP_OR                 "OR"
#define TOK_OP_AND                "AND"
#define TOK_OP_NOT                "NOT"

#define TOK_OPS \
  X(TOK_OP_COMMA, to_Comma) \
  X(TOK_OP_LPAREN, to_LParen) \
  X(TOK_OP_RPAREN, to_RParen) \
  X(TOK_OP_PLUS, to_Plus) \
  X(TOK_OP_MINUS, to_Minus) \
  X(TOK_OP_MULT, to_Mult) \
  X(TOK_OP_DIV, to_Div) \
  X(TOK_OP_POW, to_Pow) \
  X(TOK_OP_LT, to_LT) \
  X(TOK_OP_GT, to_GT) \
  X(TOK_OP_LE, to_LE) \
  X(TOK_OP_GE, to_GE) \
  X(TOK_OP_EQ, to_Eq) \
  X(TOK_OP_NEQ, to_NEq) \
  X(TOK_OP_OR, to_Or) \
  X(TOK_OP_AND, to_And) \
  X(TOK_OP_NOT, to_Not) \

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

#define TOK_KEYOWRDS \
  X(KEYWORD_LET, tk_Let) \
  X(KEYWORD_PRINT, tk_Print) \
  X(KEYWORD_INPUT, tk_Input) \
  X(KEYWORD_GOTO, tk_Goto) \
  X(KEYWORD_GOSUB, tk_Gosub) \
  X(KEYWORD_RETURN, tk_Return) \
  X(KEYWORD_END, tk_End) \
  X(KEYWORD_IF, tk_If) \
  X(KEYWORD_THEN, tk_Then) \
  X(KEYWORD_REM, tk_Remark) \

#define FUNCTION_SIN              "SIN"

#define TOK_FUNCTIONS \
  X(FUNCTION_SIN, tf_Sin) \

typedef enum {
  tt_Unknown,
  tt_None,
  tt_Function,
  tt_Variable,
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
#define X(TOK_KW, TOK_ENUM) TOK_ENUM,
  TOK_KEYOWRDS
#undef X
} TokenKeyword;

typedef enum {
  tf_None,
#define X(TOK_FUNC, TOK_ENUM) TOK_ENUM,
  TOK_FUNCTIONS
#undef X
} TokenFunction;

typedef struct {
  size_t argCount;
  TokenFunction func;
} TokenFunctionCall;

typedef struct {
  TokenType type;
  union {
    Str string;
    TokenFunctionCall func;
    TokenOperator operator;
    TokenKeyword keyword;
    float _float;
    long _long;
  };
} Token;


void tok_free(Token* token);
int tok_duplicate(Token* token, const Token* src);
char* tok_get_type_cstr(TokenType type);
void tok_print(Token* token);

const char* tok_keyword_get_cstr(TokenKeyword tk);
const char* tok_operator_get_cstr(TokenOperator to);
const char* tok_function_get_cstr(TokenFunction tf);

TokenKeyword tok_get_keyword(const char* cstr, size_t len);
TokenOperator tok_get_operator(const char* cstr, size_t max_len);
TokenFunction tok_get_function(const char* cstr, size_t len);
int tok_get_operator_precedence(TokenOperator op);

#endif
