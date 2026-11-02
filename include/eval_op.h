#ifndef EVAL_OP_H
#define EVAL_OP_H

#include <tokens.h>
#include <interpreter.h>


int evl_eval_op(Itp* itp, Token* token, Vec* stack);

#endif
