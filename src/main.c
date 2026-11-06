#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "result.h"
#include "str.h"


char* window_title = "terraTree";
Itp itp;

void sigint_handler(int sig) {
  (void)sig;

  if (itp.running) {
    itp.running = false;
  } else {
    exit(0);
  }
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  if (itp_init(&itp)) return _res;

  signal(SIGINT, sigint_handler);

  while (true) {
    if (itp_shell(&itp)) {
      if (str_cstr(&itp.errorString)) return _res;
      printf("Error: %s\n", itp.errorString.text);
    }
  }

  itp_free(&itp);

  return 0;
}

