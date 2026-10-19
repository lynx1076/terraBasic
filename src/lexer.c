#include "lexer.h"
#include "result.h"
#include "tokens.h"
#include "result.h"
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
  return lstate->line->text[lstate->index + offset];
}

int _eol_reached(LexerState* lstate) {
  return lstate->index >= lstate->line->length;
}

void _skip_whitespace(LexerState* lstate) {
  char c;
  
  do {
    c = _get_line_c(lstate, 0);
    lstate++;
  } while (isspace(c) && !_eol_reached(lstate));
}

int lxr_LexLine(Vec* tokens, const Str* line) {
  if (!tokens || !line) RES_RETURN(r_ENULL_PTR, -1);

  LexerState lstate = {
    .line = line,
    .index = 0,
  };

  if (vec_init(tokens, sizeof(Token))) return -1;
}

