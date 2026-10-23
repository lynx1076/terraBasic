#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>
#include "vector.h"
#include "str.h"

typedef struct {
  Str name;
  union {
    Str string;
    float* array;
    float value;
  };
} ItpVar;

typedef struct {
  size_t line_num;
  Str text;
  Vec tokens;
} ItpLine;

typedef struct {
  size_t lineIndex;
  Vec lines;
  Vec variables;
  Str errorString;
} Itp;

int itp_init(Itp* itp);
void itp_free(Itp* itp);

int itp_line_add(ItpLine* line, const Str* text, size_t line_num);

#endif
