#ifndef INTERPRETER_HELPERS_H
#define INTERPRETER_HELPERS_H

#include "interpreter.h"

ItpVar* itp_get_var(const Itp* itp, const Str* name);
int handle_variable(Itp* itp);
int handle_keyword(Itp* itp);

int keyword_none(Itp* itp, Vec* tokens);
int keyword_let(Itp* itp, Vec* tokens);
int keyword_print(Itp* itp, Vec* tokens);
int keyword_input(Itp* itp, Vec* tokens);
int keyword_goto(Itp* itp, Vec* tokens);
int keyword_gosub(Itp* itp, Vec* tokens);
int keyword_return(Itp* itp, Vec* tokens);
int keyword_end(Itp* itp, Vec* tokens);
int keyword_if(Itp* itp, Vec* tokens);
int keyword_else(Itp* itp, Vec* tokens);
int keyword_then(Itp* itp, Vec* tokens);
int keyword_colon(Itp* itp, Vec* tokens);

#endif
