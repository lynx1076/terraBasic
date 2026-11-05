#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"


char* window_title = "terraTree";

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  Str error;
  if (str_init_cstr(&error, "Unknown error")) return _res;

  Itp itp;
  if (itp_init(&itp)) return _res;

  Str input_str;
  if (str_init(&input_str)) return _res;

  while (true) {
    if (itp_shell(&itp)) {
      if (str_cstr(&itp.errorString)) return _res;
      printf("Failed to interpret line: %s\n", itp.errorString.text);
    }
  }

  itp_free(&itp);
  str_free(&error);
  str_free(&input_str);

  return 0;
}

