#include "interpreter.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"
#include <stddef.h>

void var_free(ItpVar* var);
int _interpret_keyword(Itp* itp);

int itp_line_init(ItpLine* line, Str* text, size_t line_num) {
  ASSERT(line);
}


void var_free(ItpVar* var) {
    
}

int _interpret_keyword(Itp* itp, Token* token) {
  
}

int itp_init(Itp* itp) {
  ASSERT(itp);

  if (vec_init(&itp->lines, sizeof(Str))) return -1;
  if (vec_init(&itp->variables, sizeof(ItpVar))) return -1;

  itp->lineIndex = 0;
  
  RES_RETURN(r_ENONE, 0);
}

void itp_free(Itp* itp) {
  ASSERT(itp);

  for (size_t line = 0; line < itp->lines.count; line++) {
    str_free(vec_get(&itp->lines, line));
  }

  for (size_t var = 0; var < itp->variables.count; var++) {
    var_free(vec_get(&itp->variables, var));
  }
}

int itp_interpret(Itp* itp, Vec* tokens, char** errorString) {
  ASSERT(itp);
  ASSERT(tokens);

  if (errorString) *errorString = "Unknown error";

  if (!tokens->count) RES_RETURN(r_ENONE, 0);

  if (tokens->count == 0) return -1;

  Token* token = vec_get(tokens, 0);

  if (token->type == tt_Keyword) {
    if (_interpret_keyword(itp, token)) return -1;
  }

  RES_RETURN(r_ENONE, 0);
}


