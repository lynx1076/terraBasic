#include "interpreter.h"
#include "interpreter_helpers.h"
#include "debug.h"
#include "lexer.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"
#include <stddef.h>
#include <time.h>

void _var_free(ItpVar* var);
void _line_free(ItpLine* line);

int itp_var_add(Itp* itp, Str* name, Token* token) {
  ASSERT(itp);
  ASSERT(name);

  for (size_t i = 0; i < itp->variables.count; i++) {
    ItpVar* check_var = vec_get(&itp->variables, i);
    if (str_cmp(&check_var->name, name)) {
      if (str_cstr(name)) return -1;
      if (str_fmt_cstr(&itp->errorString, "Variable \"%s\" already declared", name->text)) return -1;
      RES_RETURN(r_EINTERPRET, 0);
    }
  }

  ItpVar var;
  if (str_dup(&var.name, name)) return -1;
  if (token) { if (tok_duplicate(&var.token, token)) return -1; }
  else var.token.type = tt_None;
  if (vec_push(&itp->variables, &var)) return -1;

  RES_RETURN(r_ENONE, 0);
}

void _var_free(ItpVar* var) {
  ASSERT(var);

  tok_free(&var->token);
  str_free(&var->name);
}

void _line_free(ItpLine* line) {
  ASSERT(line);

  for (size_t i = 0; i < line->tokens.count; i++) {
    Token* token = vec_get(&line->tokens, i);

    tok_free(token);
  }
  vec_free(&line->tokens);
  str_free(&line->text);
}

int itp_init(Itp* itp) {
  ASSERT(itp);

  if (vec_init(&itp->lines, sizeof(ItpLine))) return -1;
  if (vec_init(&itp->variables, sizeof(ItpVar))) return -1;

  itp->lineIndex = 0;
  if (str_init_cstr(&itp->errorString, "Unknown error")) return -1;
  
  RES_RETURN(r_ENONE, 0);
}

void itp_free(Itp* itp) {
  ASSERT(itp);

  for (size_t i = 0; i < itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);

    _line_free(line);
  }

  for (size_t i = 0; i < itp->variables.count; i++) {
    ItpVar* var = vec_get(&itp->variables, i);

    _var_free(var);
  }

  vec_free(&itp->lines);
  vec_free(&itp->variables);
  str_free(&itp->errorString);
}

ItpLine* ipt_get_line(Itp* itp, size_t line_num) {
  ASSERT(itp);

  for (size_t i = 0; i <= itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);
    if (line->line_num > line_num) RES_RETURN(r_ENONE, NULL);
    else if (line->line_num == line_num) RES_RETURN(r_ENONE, line);
  }
  RES_RETURN(r_ENONE, NULL);
}

int itp_line_add(Itp* itp, const Str* text, size_t line_num) {
  ASSERT(itp);

  ItpLine new_line;
  ItpLine* new_line_ptr = NULL;

  new_line.line_num = line_num;
  
  if (str_dup(&new_line.text, text)) return -1;
  if (vec_init(&new_line.tokens, sizeof(Token))) return -1;

  for (size_t i = 0; i <= itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);

    if (!line || line_num < line->line_num) {
      if (vec_push_to(&itp->lines, i, &new_line)) return -1;
      new_line_ptr = vec_get(&itp->lines, i);
      break;
    } else if (line_num == line->line_num) {
      _line_free(line);
      *line = new_line;
      new_line_ptr = line;
      break;
    }
  }

  if (lxr_LexLine(&new_line_ptr->tokens, &itp->errorString, &new_line_ptr->text)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int itp_shell(Itp* itp) {
  ASSERT(itp);

  Str input_str;
  if (str_init(&input_str)) return _res;

  printf(">>>");
  char* line = NULL;
  size_t size = 0;
  ssize_t len = 0;

  len = getline(&line, &size, stdin) - 1;
  if (len < 0) {
    printf("Failed to get input\n");
    return -1;
  }
  line[len] = '\0';
  if (!len) RES_RETURN(r_ENONE, 0);

  ItpLine new_line;

  if (str_init_cstr(&new_line.text, line)) return -1;
  if (vec_init(&new_line.tokens, sizeof(Token))) return -1;
  if (lxr_LexLine(&new_line.tokens, &itp->errorString, &new_line.text)) return -1;

  Token* line_tok = vec_get(&new_line.tokens, 0);
  if (line_tok && line_tok->type == tt_Long) {
    long line_num = line_tok->_long;
    if (line_num < 0) {
      if (str_cpy_cstr(&itp->errorString, "Cannot have negative line number")) return -1;
      RES_RETURN(r_EINTERPRET, 0);
    }

    if (vec_pop_at(&new_line.tokens, 0, NULL)) return -1;

    for (size_t i = 0; i <= itp->lines.count; i++) {
      ItpLine* line_check = vec_get(&itp->lines, i);

      if (!line_check || (size_t)line_num < line_check->line_num) {
        if (vec_push_to(&itp->lines, i, &new_line)) return -1;
        break;
      } else if ((size_t)line_num == line_check->line_num) {
        _line_free(line_check);
        *line_check = new_line;
        break;
      }
    }
  } else {
    if (itp_interpret_line(itp, &new_line)) return -1;
    _line_free(&new_line);
  }
  RES_RETURN(r_ENONE, 0);
}

int itp_interpret_line(Itp* itp, ItpLine* line) {
  ASSERT(itp);
  ASSERT(line);

  Token* token = vec_get(&line->tokens, 0);

  switch (token->type) {
    case tt_Keyword: {
      if (handle_keyword(itp)) return -1;
    } break;
    case tt_Variable: {
      if (handle_variable(itp)) return -1;
    } break;
    default: {
      if (str_fmt_cstr(&itp->errorString, "Invalid token here: %s", tok_get_type_cstr(token->type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }
  }
  
  RES_RETURN(r_ENONE, 0);
}

int itp_print_line(Itp* itp, size_t line_num) {
  ASSERT(itp);

  for (size_t i = 0; i < itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);
    if (line->line_num != line_num) continue;

    if (str_cstr(&line->text)) return -1;

    printf("Line %zu\n", line_num);
    for (size_t i = 0; i < line->tokens.count; i++) {
      Token* token = vec_get(&line->tokens, i);
      printf("(%zu): ", i);
      tok_print(token);
      printf("\n");
    }

    RES_RETURN(r_ENONE, 0);
  }
  printf("%zu: -- EMPTY --\n", line_num);
  RES_RETURN(r_ENONE, 0);
}

