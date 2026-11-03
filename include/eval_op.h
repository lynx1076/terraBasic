#ifndef EVAL_OP_H
#define EVAL_OP_H

#include <tokens.h>
#include <interpreter.h>


int evl_eval_op(Itp* itp, Token* token, Vec* stack);
int eval_none(Itp* itp, Vec* stack);
int eval_plus(Itp* itp, Vec* stack);
int eval_minus(Itp* itp, Vec* stack);
int eval_mult(Itp* itp, Vec* stack);
int eval_div(Itp* itp, Vec* stack);
int eval_pow(Itp* itp, Vec* stack);
int eval_or(Itp* itp, Vec* stack);
int eval_and(Itp* itp, Vec* stack);
int eval_not(Itp* itp, Vec* stack);
int eval_eq(Itp* itp, Vec* stack);
int eval_neq(Itp* itp, Vec* stack);
int eval_lt(Itp* itp, Vec* stack);
int eval_gt(Itp* itp, Vec* stack);
int eval_le(Itp* itp, Vec* stack);
int eval_ge(Itp* itp, Vec* stack);

#endif
