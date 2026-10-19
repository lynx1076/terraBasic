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
} tok_TokenType;

typedef struct {
  tok_TokenType type;
  Str string;
} tok_Token;


int tok_init(tok_Token* token, const char* text, size_t length);
void tok_deinit(tok_Token* token);

#endif
