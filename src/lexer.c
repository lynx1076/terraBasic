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

int lxr_LexLine(Vec* tokens, const char** error_string, const Str* line) {
  ASSERT(!tokens || !line);

  LexerState lstate = {
    .line = line,
    .index = 0,
  };

  if (error_string) *error_string = "Unknown error";

  while (!_eol_reached(&lstate)) {
    char c = _get_cursor(&lstate, 0);

    size_t tokenStart = lstate.index;
    size_t tokenEnd = lstate.index;
    TokenType type = tt_None;

    if (isalpha(c)) {
      while (isalnum(c) && !_eol_reached(&lstate)) c = _move_cursor(&lstate, 1);
      tokenEnd = lstate.index;

      _move_cursor(&lstate, -1);
      
      type = tt_Identifier;
    } else if (isdigit(c)) {
      bool hadDecimalPoint = false;
      while (isdigit(c) || (c == SIGCHAR_DECIMAL_POINT && !hadDecimalPoint)) {
        if (c == SIGCHAR_DECIMAL_POINT) {
          if (hadDecimalPoint) {
            if (error_string) *error_string = "Number may not have multiple decimal points";
            RES_RETURN(r_ELEX, -1);
          }
          hadDecimalPoint = true;
        }
        c = _move_cursor(&lstate, 1);
        if (_eol_reached(&lstate)) break;
      }

      tokenEnd = lstate.index;
      _move_cursor(&lstate, -1);

      type = tt_Number;
    } else if (c == SIGCHAR_STRING_BOUNDARY) {
      tokenStart++;
      do {
        if (_eol_reached(&lstate)) {
          if (error_string) *error_string = "String was not terminated";
          RES_RETURN(r_EPARSE, -1);
        }
        c = _move_cursor(&lstate, 1);
      } while (c != SIGCHAR_STRING_BOUNDARY);

      tokenEnd = lstate.index;
      type = tt_String;
    } else if (isspace(c)) {
      _skip_whitespace(&lstate);
      type = tt_None;
    } else {
      switch (c) {
      case TOKCHAR_COMMA: type = tt_Comma; break;
      case TOKCHAR_LPAREN: type = tt_LParen; break;
      case TOKCHAR_RPAREN: type = tt_RParen; break;
      case TOKCHAR_OP_PLUS: type = tt_Operator; break;
      case TOKCHAR_OP_MINUS: type = tt_Operator; break;
      case TOKCHAR_OP_MULT: type = tt_Operator; break;
      case TOKCHAR_OP_DIV: type = tt_Operator; break;
      case TOKCHAR_OP_POW: type = tt_Operator; break;
      default: {
        if (error_string) *error_string = "Unexpected character discovered";
        RES_RETURN(r_EPARSE, -1);
      }
      }

      tokenEnd = lstate.index + 1;
    }

    switch (type) {
    case tt_None: break;
    case tt_Unknown: {
      *error_string = "Encountered an unknown token";
    } RES_RETURN(r_EPARSE, -1);
    default: {
      Token token;
      if (tok_init(&token, type, lstate.line->text + tokenStart, tokenEnd - tokenStart)) return -1;
      if (vec_push(tokens, &token)) return -1;
    } break;
    }

    _move_cursor(&lstate, 1);
  }

  RES_RETURN(r_ENONE, 0);
}

