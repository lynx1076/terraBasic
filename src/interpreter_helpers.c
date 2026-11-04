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

int handle_keyword(Itp* itp) {
  ASSERT(itp);

  Vec* tokens = &((ItpLine*)vec_get(&itp->lines, itp->lineIndex))->tokens;
  Token* token = vec_get(tokens, 0);
  ASSERT(token->type == tt_Keyword);

  switch (token->keyword) {
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_EVAL(itp, stack, token);
    TOK_KEYWORDS
#undef X
  default:
    RES_RETURN(r_EUNREACHABLE, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int keyword_none(Itp* itp, Vec* tokens) {
  (void)itp;
  (void)tokens;

  printf("Tried to evaluate keyword that should not be evaluated here!\n");
  ASSERT(0);
}

int keyword_let(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  Token* var_tok = vec_get(tokens, 2);

  if (var_tok->type == tt_Variable) {
    if (str_cstr(&var_tok->string)) return -1;
    if (str_fmt_cstr(&itp->errorString, "\"%s\" is not a variable", var_tok->string.text)) return -1;

    RES_RETURN(r_EINTERPRET, -1);
  }

  ItpVar* var = itp_get_var(itp, &var_tok->string);

  if (!var) {
    if (str_cstr(&var_tok->string)) return -1;
    if (str_fmt_cstr(&itp->errorString, "\"%s\" is not a known variable", var_tok->string.text)) return -1;

    RES_RETURN(r_EINTERPRET, -1);
  }

  Token* token = vec_get(tokens, 1);
  if (!token || token->type != tt_Operator || token->operator != to_Eq) {
    if (str_fmt_cstr(&itp->errorString, "Expected assignment after variable", token->string)) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (evl_eval(itp, 2, tokens->count, &var->token)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int keyword_print(Itp* itp, Vec* tokens) {
}

int keyword_input(Itp* itp, Vec* tokens) {
}

int keyword_goto(Itp* itp, Vec* tokens) {
}

int keyword_gosub(Itp* itp, Vec* tokens) {
}

int keyword_return(Itp* itp, Vec* tokens) {
}

int keyword_end(Itp* itp, Vec* tokens) {
}

int keyword_if(Itp* itp, Vec* tokens) {
}

int keyword_else(Itp* itp, Vec* tokens) {
}

int keyword_then(Itp* itp, Vec* tokens) {
}

int keyword_colon(Itp* itp, Vec* tokens) {
}

