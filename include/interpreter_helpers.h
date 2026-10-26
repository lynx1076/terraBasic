#ifndef INTERPRETER_HELPERS_H
#define INTERPRETER_HELPERS_H

#include "interpreter.h"

ItpVar* itp_get_var(const Itp* itp, const Str* name);
int handle_variable(Itp* itp);
int handle_keyword(Itp* itp);

#endif
