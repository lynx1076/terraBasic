#include <stddef.h>
#include <stdio.h>
#include "interpreter.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"


char* window_title = "terraTree";

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  Vec tokens;
  if (vec_init(&tokens, sizeof(Token))) return _res;

  Str error;

  if (str_init_cstr(&error, "Unknown error")) return _res;

  Str line_str;
  if (str_init_cstr(&line_str, "a = (1 + 2) * (2 + b)")) return _res;

  if (str_cstr(&line_str)) return _res;
  printf("Processing string (%zu): %s\n", line_str.length, line_str.text);

  Itp itp;
  if (itp_init(&itp)) return _res;

  if (itp_line_add(&itp, &line_str, 1)) {
    if (str_cstr(&itp.errorString)) return _res;
    printf("Failed to add line: %s\n", itp.errorString.text);
    return _res;
  }
  if (itp_print_line(&itp, 1)) return _res;

  ItpVar var;
  if (str_init_cstr(&var.name, "a")) return _res;
  var.type = itv_Float;
  var._float = 20;

  if (vec_push(&itp.variables, &var)) return _res;

  if (itp_interpret_line(&itp, 0)) {
    if (str_cstr(&itp.errorString)) return _res;
    printf("Failed to interpret line: %s\n", itp.errorString.text);
    return _res;
  }

  return 0;
}

