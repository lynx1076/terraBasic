#include "tokens.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>


void tok_free(Token* token) {
  ASSERT(token);

  if (token->type == tt_String || token->type == tt_Identifier) {
    str_free(&token->string);
  } else if (token->type == tt_Function) {
    str_free(&token->func.name);
  }
}

char* tok_get_type_cstr(TokenType type) {
  switch (type) {
#define CASE(X) case X: return (char*)#X + 3
  CASE(tt_None);
  CASE(tt_Identifier);
  CASE(tt_Float);
  CASE(tt_Long);
  CASE(tt_Comma);
  CASE(tt_Dot);
  CASE(tt_String);
  CASE(tt_Operator);
  CASE(tt_Keyword);
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
      printf("%s => %s", tok_get_type_cstr(token->type), tok_get_operator_cstr(token->operator));
    } break;
    case tt_Identifier:
    case tt_String: {
      str_cstr(&token->string);
      printf("%s => \"%s\"", tok_get_type_cstr(token->type), token->string.text);
    } break;
    case tt_Keyword: {
      printf("%s => %s", tok_get_type_cstr(token->type), tok_tk_get_cstr(token->keyword));
    } break;
    case tt_Float: {
      printf("%s => %f", tok_get_type_cstr(token->type), token->_float);
    } break;
    case tt_Long: {
      printf("%s => %li", tok_get_type_cstr(token->type), token->_long);
    } break;
    default: {
      printf("%s", tok_get_type_cstr(token->type));
    } break;
  }
}

const char* tok_get_operator_cstr(TokenOperator op) {
  switch (op) {
#define X(TOK_OP, TOK_ENUM) case TOK_ENUM: return TOK_OP;
    TOK_OPS
#undef X
    default: return "UNKNOWN";
  }
}

TokenKeyword tok_get_keyword(const char* cstr, size_t len) {
  ASSERT(cstr);
  
  if (!len) RES_RETURN(r_ENONE, tk_None);
  
  #define TEST(KEYWORD, TOKEN_KEYWORD)   if (!strncicmp((KEYWORD), cstr, len)) RES_RETURN(r_ENONE, (TOKEN_KEYWORD));

  TEST(KEYWORD_LET, tk_Let);
  TEST(KEYWORD_PRINT, tk_Print);
  TEST(KEYWORD_INPUT, tk_Input);
  TEST(KEYWORD_GOTO, tk_Goto);
  TEST(KEYWORD_GOSUB, tk_Gosub);
  TEST(KEYWORD_RETURN, tk_Return);
  TEST(KEYWORD_END, tk_End);
  TEST(KEYWORD_IF, tk_If);
  TEST(KEYWORD_THEN, tk_Then);
  TEST(KEYWORD_REM, tk_Remark);

  RES_RETURN(r_ENONE, tk_None);
}

char* tok_tk_get_cstr(TokenKeyword tk) {
  #define CASE(KEYWORD)   case (KEYWORD): return (char*)(#KEYWORD) + 3

  switch (tk) {
  CASE(tk_Let);
  CASE(tk_Print);
  CASE(tk_Input);
  CASE(tk_Goto);
  CASE(tk_Gosub);
  CASE(tk_Return);
  CASE(tk_End);
  CASE(tk_If);
  CASE(tk_Then);
  CASE(tk_Remark);
  CASE(tk_None);
  default: return "Unknown";
  }
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
    case to_CmpEq:
    case to_CmpL:
    case to_CmpG:
    case to_CmpLT:
    case to_CmpGT:
      return 100;
    case to_LParen:
    case to_RParen:
    case to_None:
    default:
      return 0;
  }
}

