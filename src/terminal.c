#include "debug.h"
#include "result.h"
#include "str.h"
#include <ctype.h>
#include <signal.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>
#include <termios.h>
#include <stdbool.h>

struct termios orig_termios;
atomic_bool terminal_int = false;

void _sigint_handler(int sig) {
  (void)sig;

  terminal_int = true;
}

void _reset_terminal_mode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

bool got_interrupt(void) {
  bool cpy = terminal_int;
  terminal_int = false;

  return cpy;
}

void set_conio_mode(void) {
  struct termios new_termios;
  
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(_reset_terminal_mode);
  
  new_termios = orig_termios;
  new_termios.c_lflag &= ~(ICANON | ECHO);
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 0;
  
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);

  signal(SIGINT, _sigint_handler);
}

int get_input(Str* str) {
  ASSERT(str);

  if (str_clear(str)) return -1;

  while (!got_interrupt()) {
    char ch;
    int bytes_read = read(STDIN_FILENO, &ch, 1);

    if (bytes_read > 0) {
      if (ch == '\n' || ch == '\r') {
        printf("\n");
        RES_RETURN(r_ENONE, 0);
      } else if (ch == 127 || ch == 8) { // Backspace
        if (str->length > 0) {
          if (str_substr(str, 0, str->length - 1)) return -1;
          printf("\b \b"); // Move back, print space, move back
          fflush(stdout);
        }
      } else if (isprint(ch)) {
        if (str_cncat(str, &ch, 1)) return -1;
        putchar(ch);
        fflush(stdout);
      }
    }

    usleep(10000);
  }
  printf("\n");

  RES_RETURN(r_INTERRUPTED, -1);
}

