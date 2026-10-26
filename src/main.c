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
  Str line;

  if (str_init_cstr(&error, "Unknown error")) return _res;

  if (str_init_cstr(&line, "a = 20 * 2 + 3 * 4")) return _res;
  str_cstr(&line);
  printf("Processing string (%zu): %s\n", line.length, line.text);

  Itp itp;
  if (itp_init(&itp)) return _res;

  ItpVar var;
  var.type = itv_Float;
  if (str_init_cstr(&var.name, "a")) return _res;

  if (vec_push(&itp.variables, &var)) return _res;

  if (itp_line_add(&itp, &line, 1)) return _res;
  if (itp_print_line(&itp, 1)) return _res;

  printf("\n\n");

  if (itp_interpret_line(&itp, 0)) printf("%s\n", res_get_string(_res));

  return 0;
}

