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

int tok_duplicate(Token* token, const Token* src) {
  ASSERT(token);
  ASSERT(src);

  *token = *src;
  if (token->type != tt_String && token->type != tt_Variable) RES_RETURN(r_ENONE, 0);

  if (str_dup(&token->string, &src->string)) return -1;

  RES_RETURN(r_ENONE, 0);
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
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_STR;
  TOK_KEYWORDS
#undef X
  
  default: return "Unknown keyword";
  }
}

const char* tok_operator_get_cstr(TokenOperator to) {
  switch (to) {
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_STR;
    TOK_OPS
#undef X
    default: return "Unknown operator";
  }
}

const char* tok_function_get_cstr(TokenFunction tf) {
  switch (tf) {
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_STR;
    TOK_FUNCTIONS
#undef X
    default: return "Unknown function";
  }
}

TokenKeyword tok_get_keyword(const char* cstr, size_t len) {
  ASSERT(cstr);
  
  if (!len) RES_RETURN(r_ENONE, tk_None);
 
#define X(TOK_STR, TOK_ENUM, TOK_EVAL)   if (!strncicmp((TOK_STR), cstr, len)) RES_RETURN(r_ENONE, (TOK_ENUM));
  TOK_KEYWORDS
#undef X

  RES_RETURN(r_ENONE, tk_None);
}

TokenFunction tok_get_function(const char* cstr, size_t len) {
  ASSERT(cstr);
  
  if (!len) RES_RETURN(r_ENONE, tf_None);
 
#define X(TOK_STR, TOK_ENUM, TOK_EVAL)   if (!strncicmp((TOK_STR), cstr, len)) RES_RETURN(r_ENONE, (TOK_ENUM));
  TOK_FUNCTIONS
#undef X

  RES_RETURN(r_ENONE, tf_None);
}

int _tok_try_operator(const char* cstr, size_t max_len, const char* suspect_cstr) {
  bool str_op = isalpha(*cstr);

  int match_len = 0;

  while (*suspect_cstr && max_len--) {
    if (tolower(*suspect_cstr) != tolower(*cstr)) return 0;
    cstr++;
    suspect_cstr++;
    match_len++;
  }

  if (*suspect_cstr != '\0') return 0;

  if (str_op && (isalpha(*cstr) || *cstr == '_')) return 0;
  return match_len;
}

TokenOperator tok_get_operator(const char* cstr, size_t max_len) {
  ASSERT(cstr);
  
  TokenOperator op = to_None;
  int longest_match = 0;
  int new_match;

#define X(TOK_STR, TOK_ENUM, TOK_EVAL) \
  new_match = _tok_try_operator(cstr, max_len, (TOK_STR)); \
  if (new_match != to_None && new_match > longest_match) { \
    longest_match = new_match; \
    op = TOK_ENUM; \
  }
  TOK_OPS
#undef X

  RES_RETURN(r_ENONE, op);
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

int tok_print_value(Token* token) {
  ASSERT(token);

  switch (token->type) {
    case tt_Variable:
    case tt_String: {
      if (str_cstr(&token->string)) return -1;
      printf("%s", token->string.text);
    } break;
    case tt_Float: {
      printf("%f", token->_float);
    } break;
    case tt_Long: {
      printf("%li", token->_long);
    } break;
    default: {
      printf("Undefined");
    } break;
  }

  RES_RETURN(r_ENONE, 0);
}

bool tok_is_true(Token* token) {
  ASSERT(token);

  switch (token->type) {
    case tt_Float:
      return token->_float != 0;
    case tt_Long:
      return token->_long != 0;
    case tt_String:
      return token->string.length != 0;
    default:
      return false;
  }
}

