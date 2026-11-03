#ifndef EVAL_H
#define EVAL_H

#include <stddef.h>
#include "interpreter.h"
#include "tokens.h"


int evl_eval(Itp* itp, size_t start, size_t stop, Token* res);

#endif
