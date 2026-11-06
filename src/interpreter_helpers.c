#include "interpreter_helpers.h"
#include "eval.h"
#include "interpreter.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>

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

  Vec* tokens = &itp->current_line->tokens;
  Token* var_tok = vec_get(tokens, 0);

  if (!var_tok || var_tok->type != tt_Variable) {
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

  Token* token_eq = vec_get(tokens, 1);
  if (!token_eq || token_eq->type != tt_Operator || token_eq->operator != to_Eq) {
    if (str_fmt_cstr(&itp->errorString, "Expected assignment after variable")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (evl_eval(itp, 2, tokens->count, &var->token)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int handle_keyword(Itp* itp) {
  ASSERT(itp);

  Vec* tokens = &itp->current_line->tokens;
  Token* token = vec_get(tokens, 0);
  ASSERT(token->type == tt_Keyword);

  switch (token->keyword) {
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_EVAL(itp, tokens);
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

int keyword_list(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  printf("%zu line%s:\n", itp->lines.count, itp->lines.count == 1 ? "" : "s");
  for (size_t i = 0; i < itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);
    if (str_cstr(&line->text)) return -1;
    printf("%s\n", line->text.text);
  }
  printf("Done\n");

  RES_RETURN(r_ENONE, 0);
}

int keyword_run(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  if (itp->running) {
    if (str_cpy_cstr(&itp->errorString, "Cannot run program from within itsself")) return -1;
    itp->running = false;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (itp->lines.count == 0) {
    if (str_cpy_cstr(&itp->errorString, "Cannot run empty program")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  itp->line_index = 0;
  itp->current_line = vec_get(&itp->lines, 0);

  itp->jumped = false;
  itp->running = true;

  RES_RETURN(r_ENONE, 0);
}

int keyword_let(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  if (tokens->count <= 3) {
    if (str_fmt_cstr(&itp->errorString, "Malformed assignment")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token* var_tok = vec_get(tokens, 1);

  if (!var_tok || var_tok->type != tt_Variable) {
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

  Token* token_eq = vec_get(tokens, 2);
  if (!token_eq || token_eq->type != tt_Operator || token_eq->operator != to_Eq) {
    if (str_fmt_cstr(&itp->errorString, "Expected assignment after variable", token_eq->string)) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (evl_eval(itp, 3, tokens->count, &var->token)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int keyword_dim(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  Token* var_tok = vec_get(tokens, 1);

  if (!var_tok || var_tok->type != tt_Variable) {
    if (str_cstr(&var_tok->string)) return -1;
    if (str_fmt_cstr(&itp->errorString, "\"%s\" is not a variable name", var_tok->string.text)) return -1;

    RES_RETURN(r_EINTERPRET, -1);
  }

  if (itp_var_add(itp, &var_tok->string, NULL)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int keyword_print(Itp* itp, Vec* tokens) {
  ASSERT(itp);
  ASSERT(tokens);

  Token print_tok;

  if (evl_eval(itp, 1, tokens->count, &print_tok)) return -1;
  tok_print_value(&print_tok);
  printf("\n");
  tok_free(&print_tok);

  RES_RETURN(r_ENONE, 0);
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

