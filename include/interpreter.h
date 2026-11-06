#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>
#include "tokens.h"
#include "vector.h"
#include "str.h"

typedef struct {
  size_t line_num;
  Str text;
  Vec tokens;
} ItpLine;

typedef struct {
  size_t line_index;
  ItpLine* current_line;
  bool jumped;
  Vec lines;
  Vec variables;
  Str errorString;
  bool running;
} Itp;

typedef struct {
  Str name;
  Token token;
} ItpVar;

int itp_init(Itp* itp);
void itp_free(Itp* itp);
ItpLine* ipt_get_line(Itp* itp, size_t line_num);

int itp_var_add(Itp* itp, Str* name, Token* token);
int itp_line_add(Itp* itp, const Str* text, size_t line_num);
int itp_shell(Itp* itp);
int itp_interpret_line(Itp* itp);
int itp_print_line(Itp* itp, size_t line_num);

#endif
