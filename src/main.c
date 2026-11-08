#include <stddef.h>
#include "interpreter.h"
#include "result.h"
#include "terminal.h"


char* window_title = "terraTree";
Itp itp;

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  if (itp_init(&itp)) return _res;

  set_conio_mode();

  while (true) {
    if (itp_shell(&itp)) {
      if (_res == r_INTERRUPTED) break;
    }
  }

  itp_free(&itp);

  return 0;
}

