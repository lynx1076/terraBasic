#ifndef TOKENS_H
#define TOKENS_H

#include "str.h"
#include <stddef.h>

typedef enum {
  tt_Unknown,
  tt_Identifier,
  tt_Builtin,
  tt_Number,
  tt_String,
  tt_Operator,
} TokenType;

typedef struct {
  TokenType type;
  Str string;
} Token;


int tok_init(Token* token, const char* text, size_t length);
void tok_free(Token* token);

#endif
