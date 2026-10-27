#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stddef.h>
#include "vector.h"
#include "str.h"

typedef enum {
  itv_None,
  itv_Float,
  itv_String,
  itv_Array,
} ItpVarType;

typedef struct {
  Str name;
  ItpVarType type;
  union {
    Str string;
    float* array;
    long _long;
    float _float;
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

int itp_line_add(Itp* itp, const Str* text, size_t line_num);
int itp_interpret_line(Itp* itp, size_t i);
int itp_print_line(Itp* itp, size_t line_num);

#endif
