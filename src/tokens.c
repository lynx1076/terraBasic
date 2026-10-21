#include "tokens.h"
#include "debug.h"
#include "result.h"
#include <stdio.h>


int tok_init(Token* token, TokenType type, const char* text, size_t length) {
  ASSERT(token && text);

  if (str_init_cnstr(&token->string, text, length)) return -1;

  token->type = type;

  RES_RETURN(r_ENONE, 0);
}

void tok_free(Token* token) {
  str_free(&token->string);
}

char* tok_get_type_cstr(TokenType type) {
  switch (type) {
#define CASE(X) case X: return (char*)#X + 3
  CASE(tt_None);
  CASE(tt_Identifier);
  CASE(tt_Number);
  CASE(tt_Comma);
  CASE(tt_Dot);
  CASE(tt_String);
  CASE(tt_LParen);
  CASE(tt_RParen);
  CASE(tt_Operator);
  default:
  CASE(tt_Unknown);
  }
}

void tok_print(Token* token) {
  if (!token) {
    printf("NULL TOKEN");
    return;
  }

  str_cstr(&token->string);
  printf("%s => \"%s\"", tok_get_type_cstr(token->type), token->string.text);
}

