#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>
#include "terminal.h"
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
  Terminal* term;
  ItpLine* current_line;
  bool jumped;
  Vec lines;
  Vec variables;
  Vec sub_stack;
  Str error_string;
  Str input_string;
  bool running;
} Itp;

typedef struct {
  Str name;
  Token token;
} ItpVar;

int itp_init(Itp* itp);
void itp_free(Itp* itp);
int itp_clear(Itp* itp);
ItpLine* itp_get_line(Itp* itp, size_t line_num, size_t* index);

int itp_var_add(Itp* itp, Str* name, Token* token);
int itp_line_add(Itp* itp, const Str* text, size_t line_num);
int itp_shell(Itp* itp);
int itp_interpret_line(Itp* itp);

int itp_goto(Itp* itp, size_t line_num);
int itp_gosub(Itp* itp, size_t line_num);
int itp_return(Itp* itp);
int itp_print_line(Itp* itp, ItpLine* line);

#endif
