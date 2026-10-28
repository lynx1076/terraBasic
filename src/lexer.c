#include "lexer.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include "result.h"
#include "debug.h"
#include "utils.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  const Str* line;
  size_t index;
} LexerState;

char _get_cursor(LexerState* lstate, int offset);
char _move_cursor(LexerState* lstate, int offset);
int _eol_reached(LexerState* lstate);
void _skip_whitespace(LexerState* lstate);


char _get_cursor(LexerState* lstate, int offset) {
  ASSERT(lstate);

  return lstate->line->text[lstate->index + offset];
}

char _move_cursor(LexerState* lstate, int offset) {
  ASSERT(lstate);

  lstate->index += offset;
  return lstate->line->text[lstate->index];
}

int _eol_reached(LexerState* lstate) {
  ASSERT(lstate);

  return lstate->index >= lstate->line->length;
}

void _skip_whitespace(LexerState* lstate) {
  ASSERT(lstate);

  char c = _get_cursor(lstate, 0);
  
  while (isspace(c) && !_eol_reached(lstate)) {
    c = _move_cursor(lstate, 1);
  }
  _move_cursor(lstate, -1);
}

int lxr_LexLine(Vec* tokens, Str* error_string, const Str* line) {
  ASSERT(tokens);
  ASSERT(line);

  LexerState lstate = {
    .line = line,
    .index = 0,
  };


  while (!_eol_reached(&lstate)) {
    char c = _get_cursor(&lstate, 0);

    size_t tokenStart = lstate.index;
    Token token;

    if (isalpha(c)) {
      while (isalnum(c) && !_eol_reached(&lstate)) c = _move_cursor(&lstate, 1);

      _move_cursor(&lstate, -1);
      
      token.keyword = tok_get_keyword(lstate.line->text + tokenStart, lstate.index - tokenStart);
      if (token.keyword != tk_None) {
        token.type = tt_Keyword;
      }
      else token.type = tt_Identifier;
    } else if (isdigit(c)) {
      bool hadDecimalPoint = false;
      while (isdigit(c) || (c == SIGCHAR_DECIMAL_POINT && !hadDecimalPoint)) {
        if (c == SIGCHAR_DECIMAL_POINT) {
          if (hadDecimalPoint) {
            str_cpy_cstr(error_string, "Number may not have multiple decimal points");
            RES_RETURN(r_ELEX, -1);
          }
          hadDecimalPoint = true;
        }
        c = _move_cursor(&lstate, 1);
        if (_eol_reached(&lstate)) break;
      }

      _move_cursor(&lstate, -1);

      if (hadDecimalPoint) {
        token.type = tt_Float;

        if (ParseFloat(&token._float, lstate.line->text + tokenStart, lstate.index - tokenStart + 1)) return -1;
      } else {
        token.type = tt_Long;

        if (ParseLong(&token._long, lstate.line->text + tokenStart, lstate.index - tokenStart + 1)) return -1;
      }
    } else if (c == SIGCHAR_STRING_BOUNDARY) {
      tokenStart++;
      do {
        if (_eol_reached(&lstate)) {
          if (str_cpy_cstr(error_string, "String was not terminated")) return -1;
          RES_RETURN(r_ELEX, -1);
        }
        c = _move_cursor(&lstate, 1);
      } while (c != SIGCHAR_STRING_BOUNDARY);

      token.type = tt_String;
    } else if (isspace(c)) {
      _skip_whitespace(&lstate);
      token.type = tt_None;
    } else {
      token.type = tt_Operator;
      if (0) {}
#define X(TOK_OP, TOK_ENUM) else if (!strncmp(TOK_OP, lstate.line->text + tokenStart, strlen(TOK_OP))) token.operator = TOK_ENUM;
      TOK_OPS
#undef X
      else {
        token.type = tt_Unknown;
      }
    }

    switch (token.type) {
    case tt_None: break;
    case tt_Unknown: {
      str_cpy_cstr(error_string, "Encountered an unknown character");
    } RES_RETURN(r_EPARSE, -1);
    case tt_String:
    case tt_Identifier: {
      if (str_init_cnstr(&token.string, lstate.line->text + tokenStart, lstate.index - tokenStart + 1)) return -1;
    } // Fall through
    default: {
      if (vec_push(tokens, &token)) return -1;
    } break;
    }

    _move_cursor(&lstate, 1);
  }

  RES_RETURN(r_ENONE, 0);
}

