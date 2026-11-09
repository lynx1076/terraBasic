#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>

#define TERMINAL_WIDTH        80
#define TERMINAL_HEIGHT       25

int term_init(const char* window_title);
bool got_interrupt(void);

#endif
