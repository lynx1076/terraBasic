#include "tokens.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


void tok_free(Token* token) {
  ASSERT(token);

  if (token->type != tt_String && token->type != tt_Variable) return;
  
  str_free(&token->string);
}

char* tok_get_type_cstr(TokenType type) {
  switch (type) {
#define CASE(X) case X: return (char*)#X + 3
  CASE(tt_None);
  CASE(tt_Variable);
  CASE(tt_Float);
  CASE(tt_Long);
  CASE(tt_Comma);
  CASE(tt_Dot);
  CASE(tt_String);
  CASE(tt_Operator);
  CASE(tt_Keyword);
  CASE(tt_Function);
  default:
  CASE(tt_Unknown);
#undef CASE
  }
}

void tok_print(Token* token) {
  if (!token) {
    printf("NULL TOKEN");
    return;
  }

  switch (token->type) {
    case tt_Operator: {
      printf("%s => %s", tok_get_type_cstr(token->type), tok_operator_get_cstr(token->operator));
    } break;
    case tt_Variable:
    case tt_String: {
      str_cstr(&token->string);
      printf("%s => \"%s\"", tok_get_type_cstr(token->type), token->string.text);
    } break;
    case tt_Keyword: {
      printf("%s => %s", tok_get_type_cstr(token->type), tok_keyword_get_cstr(token->keyword));
    } break;
    case tt_Float: {
      printf("%s => %f", tok_get_type_cstr(token->type), token->_float);
    } break;
    case tt_Long: {
      printf("%s => %li", tok_get_type_cstr(token->type), token->_long);
    } break;
    case tt_Function: {
      printf("%s => %s with %zu args", tok_get_type_cstr(token->type), tok_function_get_cstr(token->func.func), token->func.argCount);
    } break;
    default: {
      printf("%s", tok_get_type_cstr(token->type));
    } break;
  }
}

const char* tok_keyword_get_cstr(TokenKeyword tk) {
  switch (tk) {
#define X(STR, ENUM) case ENUM: return STR;
  TOK_KEYOWRDS
#undef X
  
  default: return "Unknown keyword";
  }
}

const char* tok_operator_get_cstr(TokenOperator to) {
  switch (to) {
#define X(STR, ENUM) case ENUM: return STR;
    TOK_OPS
#undef X
    default: return "Unknown operator";
  }
}

const char* tok_function_get_cstr(TokenFunction tf) {
  switch (tf) {
#define X(STR, ENUM) case ENUM: return STR;
    TOK_FUNCTIONS
#undef X
    default: return "Unknown function";
  }
}

TokenKeyword tok_get_keyword(const char* cstr, size_t len) {
  ASSERT(cstr);
  
  if (!len) RES_RETURN(r_ENONE, tk_None);
 
#define X(KEYWORD, TOKEN_KEYWORD)   if (!strncicmp((KEYWORD), cstr, len)) RES_RETURN(r_ENONE, (TOKEN_KEYWORD));
  TOK_KEYOWRDS
#undef X

  RES_RETURN(r_ENONE, tk_None);
}

TokenFunction tok_get_function(const char* cstr, size_t len) {
  ASSERT(cstr);
  
  if (!len) RES_RETURN(r_ENONE, tf_None);
 
#define X(FUNCTION, TOKEN_FUNCTION)   if (!strncicmp((FUNCTION), cstr, len)) RES_RETURN(r_ENONE, (TOKEN_FUNCTION));
  TOK_FUNCTIONS
#undef X

  RES_RETURN(r_ENONE, tf_None);
}

int _tok_try_operator(const char* cstr, size_t max_len, const char* suspect_cstr) {
  bool str_op = isalpha(*cstr);

  while (*suspect_cstr && max_len--) {
    if (tolower(*suspect_cstr) != tolower(*cstr)) return 0;
    cstr++;
    suspect_cstr++;
  }

  if (str_op && (isalpha(*cstr) || *cstr == '_')) return 0;
  return 1;
}

TokenOperator tok_get_operator(const char* cstr, size_t max_len) {
  ASSERT(cstr);
  
#define X(OP_CSTR, OP_ENUM)   if (_tok_try_operator(cstr, max_len, (OP_CSTR))) RES_RETURN(r_ENONE, (OP_ENUM));
  TOK_OPS
#undef X

  RES_RETURN(r_ENONE, to_None);
}

int tok_get_operator_precedence(TokenOperator op) {
  switch (op) {
    case to_Plus:
    case to_Minus:
      return 10;
    case to_Mult:
    case to_Div:
      return 20;
    case to_Pow:
      return 30;
    case to_None:
    default:
      return 0;
  }
}

