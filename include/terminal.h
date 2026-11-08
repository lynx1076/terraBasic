#ifndef TERMINAL_H
#define TERMINAL_H

#include "str.h"
#include <stdbool.h>


bool got_interrupt(void);
void set_conio_mode(void);
int get_input(Str* str);

#endif
