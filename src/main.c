#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "result.h"
#include "str.h"


char* window_title = "terraTree";
Itp itp;

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  if (itp_init(&itp)) return _res;

  signal(SIGINT, sigint_handler);

  while (true) {
    if (itp_shell(&itp)) {
      continue;
    }
  }

  itp_free(&itp);

  return 0;
}

