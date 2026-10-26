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

  char* line;
  size_t len;

  printf("Input line to lex: ");
  fflush(stdout);
  if (getline(&line, &len, stdin) == -1) RES_RETURN(r_EUNKNOWN, _res);

  Str line_str;
  if (str_init_cstr(&line_str, line)) return _res;

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

  return 0;
}

