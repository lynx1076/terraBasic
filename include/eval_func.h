#ifndef EVAL_FUNC_H
#define EVAL_FUNC_H

#include "vector.h"
#include "interpreter.h"
#include "tokens.h"

int evl_eval_func(Itp* itp, Token* token, Vec* stack);

int eval_sin(Itp* itp, Vec* stack, Token* token);
int eval_cos(Itp* itp, Vec* stack, Token* token);
int eval_tan(Itp* itp, Vec* stack, Token* token);
int eval_asin(Itp* itp, Vec* stack, Token* token);
int eval_acos(Itp* itp, Vec* stack, Token* token);
int eval_atan(Itp* itp, Vec* stack, Token* token);

int eval_abs(Itp* itp, Vec* stack, Token* token);

int eval_left(Itp* itp, Vec* stack, Token* token);
int eval_right(Itp* itp, Vec* stack, Token* token);
int eval_mid(Itp* itp, Vec* stack, Token* token);

#endif
