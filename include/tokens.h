#ifndef TOKENS_H
#define TOKENS_H

#include "str.h"
#include <stddef.h>

#define SIGCHAR_STRING_BOUNDARY   '\"'
#define SIGCHAR_DECIMAL_POINT     '.'
#define SIGSTR_REM_START          "REM"

#define TOK_OPS \
  X(",", to_Comma, eval_none) \
  X("(", to_LParen, eval_none) \
  X(")", to_RParen, eval_none) \
  X("+", to_Plus, eval_plus) \
  X("-", to_Minus, eval_minus) \
  X("*", to_Mult, eval_mult) \
  X("/", to_Div, eval_div) \
  X("^", to_Pow, eval_pow) \
  X("<", to_LT, eval_lt) \
  X(">", to_GT, eval_gt) \
  X("<=", to_LE, eval_le) \
  X(">=", to_GE, eval_ge) \
  X("=", to_Eq, eval_eq) \
  X("<>", to_NEq, eval_neq) \
  X("OR", to_Or, eval_or) \
  X("AND", to_And, eval_and) \
  X("NOT", to_Not, eval_not) \

#define TOK_KEYWORDS \
  X("REM", tk_Rem, keyword_none) \
  X("LET", tk_Let, keyword_let) \
  X("DIM", tk_Dim, keyword_dim) \
  X("PRINT", tk_Print, keyword_print) \
  X("INPUT", tk_Input, keyword_input) \
  X("GOTO", tk_Goto, keyword_goto) \
  X("GOSUB", tk_Gosub, keyword_gosub) \
  X("RETURN", tk_Return, keyword_return) \
  X("END", tk_End, keyword_end) \
  X("IF", tk_If, keyword_if) \
  X("ELSE", tk_Else, keyword_else) \
  X("THEN", tk_Then, keyword_then) \
  X(":", tk_Colon, keyword_colon) \

#define TOK_FUNCTIONS \
  X("SIN", tf_Sin, eval_sin) \
  X("COS", tf_Cos, eval_cos) \
  X("TAN", tf_Tan, eval_tan) \
  X("ASIN", tf_ASin, eval_asin) \
  X("ACOS", tf_ACos, eval_acos) \
  X("ATAN", tf_ATan, eval_atan) \
  X("ABS", tf_Abs, eval_abs) \
  X("LEFT", tf_Left, eval_left) \
  X("RIGHT", tf_Right, eval_right) \
  X("MID", tf_Mid, eval_mid) \

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
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) TOK_ENUM,
  TOK_OPS
#undef X
} TokenOperator;

typedef enum {
  tk_None,
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) TOK_ENUM,
  TOK_KEYWORDS
#undef X
} TokenKeyword;

typedef enum {
  tf_None,
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) TOK_ENUM,
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

int tok_print_value(Token* token);

#endif
