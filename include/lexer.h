#ifndef LEXER_H
#define LEXER_H

#include "str.h"
#include "vector.h"

int lxr_LexLine(Vec* tokens, const char** error_string, const Str* line);

#endif
