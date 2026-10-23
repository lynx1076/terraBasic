#include "interpreter.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"
#include <stddef.h>

void _var_free(ItpVar* var);
int _interpret_keyword(Itp* itp);
int _interpret_identifier(Itp* itp);


ItpVar* _get_var(const Itp* itp, const Str* name) {
  ASSERT(itp);
  ASSERT(name);

  for (size_t i = 0; i < itp->variables.count; i++) {
    ItpVar* var = vec_get((Vec*)&itp->variables, i);
    if (str_cmp(name, &var->name)) RES_RETURN(r_ENONE, var);
  }

  RES_RETURN(r_ENO_DATA, NULL);
}

void _var_free(ItpVar* var) {
    
}

int _interpret_keyword(Itp* itp) {

}

int _interpret_identifier(Itp* itp) {
  ASSERT(itp);

  ItpLine* line = vec_get(&itp->lines, itp->lineIndex);
  Token* first_tok = vec_get(&line->tokens, 0);

  ItpVar* var = _get_var(itp, &first_tok->string);
  if (!var) {
    str_fmt_cstr(errorString, char *fmt, ...)
  }
}

int itp_init(Itp* itp) {
  ASSERT(itp);

  if (vec_init(&itp->lines, sizeof(Str))) return -1;
  if (vec_init(&itp->variables, sizeof(ItpVar))) return -1;

  itp->lineIndex = 0;
  if (str_init_cstr(&itp->errorString, "Unknown error")) return -1;
  
  RES_RETURN(r_ENONE, 0);
}

void itp_free(Itp* itp) {
  ASSERT(itp);

  for (size_t line = 0; line < itp->lines.count; line++) {
    str_free(vec_get(&itp->lines, line));
  }

  for (size_t var = 0; var < itp->variables.count; var++) {
    _var_free(vec_get(&itp->variables, var));
  }
}

int itp_line_add(ItpLine* line, const Str* text, size_t line_num) {
  ASSERT(line);
}

int itp_interpret_line(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  if (!tokens->count) RES_RETURN(r_ENONE, 0);

  if (tokens->count == 0) return -1;

  Token* token = vec_get(tokens, 0);

  if (token->type == tt_Keyword) {
    if (_interpret_keyword(itp)) return -1;
  } else if (token->type == tt_Identifier) {
    if (_interpret_identifier(itp)) return -1;
  } else {
    str_cpy_cstr(&itp->errorString, "");
  }
  
  RES_RETURN(r_ENONE, 0);
}

