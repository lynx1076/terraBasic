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

  term.background_color = DARKBLUE;
  term.text_color = GREEN;
  if (term_init(&term, "terraBasic")) return _res;
  if (term_printf(&term, "terraBasic v%s\n", VERSION)) return _res;

  if (itp_init(&itp)) return _res;
  itp.term = &term;

  while (true) {
    if (term_update(&term)) return _res;
    if (itp_shell(&itp)) {
      if (_res != r_INTERRUPTED) return _res;
      break;
    }
  }

  itp_free(&itp);
  term_free(&term);

  return 0;
}

