#include "tokens.h"
#include "debug.h"
#include "result.h"


int tok_init(Token* token, TokenType type, const char* text, size_t length) {
  ASSERT(token && text);

  if (str_init_cnstr(&token->string, text, length)) return -1;

  token->type = type;

  RES_RETURN(r_ENONE, 0);
}

void tok_free(Token* token) {
  str_free(&token->string);
}

