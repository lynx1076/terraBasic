#include "interpreter_helpers.h"
#include "eval.h"
#include "interpreter.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"

ItpVar* itp_get_var(const Itp* itp, const Str* name) {
  ASSERT(itp);
  ASSERT(name);

  for (size_t i = 0; i < itp->variables.count; i++) {
    ItpVar* var = vec_get((Vec*)&itp->variables, i);
    if (str_cmp(name, &var->name)) RES_RETURN(r_ENONE, var);
  }

  RES_RETURN(r_ENO_DATA, NULL);
}

int handle_variable(Itp* itp) {
  ASSERT(itp);

  Vec* tokens = &((ItpLine*)vec_get(&itp->lines, itp->lineIndex))->tokens;
  
  Token* token = vec_get(tokens, 0);
  if (token->type != tt_Identifier) RES_RETURN(r_EINTERPRET, -1);

  ItpVar* var = itp_get_var(itp, &token->string);

  if (!var) {
    if (str_cstr(&token->string)) return -1;
    if (str_fmt_cstr(&itp->errorString, "\"%s\" is not a known variable", token->string)) return -1;

    RES_RETURN(r_EINTERPRET, -1);
  }

  token = vec_get(tokens, 1);
  if (!token || token->type != tt_Operator || token->operator != to_CmpEq) {
    if (str_fmt_cstr(&itp->errorString, "Expected assignment after variable", token->string)) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  evl_eval_float(itp, 2);

  RES_RETURN(r_ENONE, 0);
}

int handle_keyword(Itp* itp) {
  ASSERT(itp);

  RES_RETURN(r_ENONE, 0);
}

