#include "lexer.h"
#include "result.h"
#include "tokens.h"
#include "result.h"
#include "debug.h"
#include <ctype.h>
#include <stddef.h>

typedef struct {
  const Str* line;
  size_t index;
} LexerState;

char _get_line_c(LexerState* lstate, int offset);
int _eol_reached(LexerState* lstate);
void _skip_whitespace(LexerState* lstate);


char _get_line_c(LexerState* lstate, int offset) {
  ASSERT(lstate);

  return lstate->line->text[lstate->index + offset];
}

int _eol_reached(LexerState* lstate) {
  ASSERT(lstate);

  return lstate->index >= lstate->line->length;
}

void _skip_whitespace(LexerState* lstate) {
  ASSERT(lstate);

  char c;
  
  do {
    c = _get_line_c(lstate, 0);
    lstate++;
  } while (isspace(c) && !_eol_reached(lstate));
}

int lxr_LexLine(Vec* tokens, const Str* line) {
  ASSERT(!tokens || !line);

  LexerState lstate = {
    .line = line,
    .index = 0,
  };

  while (!_eol_reached(&lstate)) {
    char c = _get_line_c(&lstate, 0);

    size_t tokenStart = lstate.index;

    if (isalpha(c)) {
      while (isalnum(c)) lstate.index++;
      
      Token token;
      if (tok_init(&token, tt_Identifier, lstate.line->text, lstate.index - tokenStart)) return -1;

      if (vec_push(tokens, &token)) return -1;
    }
  }
}

