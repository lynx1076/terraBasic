#include <raylib.h>
#include <stddef.h>
#include "interpreter.h"
#include "result.h"
#include "terminal.h"


char* window_title = "terraTree";
Itp itp;
Terminal term;

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  if (itp_init(&itp)) return _res;

  if (term_init(&term, "terraBasic")) return _res;
  term.background_color = DARKBLUE;
  term.text_color = WHITE;

  if (term_printf(&term, "Hi from line %i\n", __LINE__)) return _res; 
  if (term_printf(&term, "Hi from line %i\n", __LINE__)) return _res; 
  if (term_printf(&term, "Hi from line %i\n", __LINE__)) return _res; 
  if (term_printf(&term, "Hi from line %i\n", __LINE__)) return _res; 

  while (true) {
    if (term_update(&term)) return _res;
  }

  itp_free(&itp);

  return 0;
}

