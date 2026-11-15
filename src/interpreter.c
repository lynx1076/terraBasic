#include "interpreter.h"
#include "interpreter_helpers.h"
#include "debug.h"
#include "lexer.h"
#include "result.h"
#include "str.h"
#include "terminal.h"
#include "vector.h"
#include "tokens.h"
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

void _var_free(ItpVar* var);
void _line_free(ItpLine* line);

Str input_str;

int itp_var_add(Itp* itp, Str* name, Token* token) {
  ASSERT(itp);
  ASSERT(name);

  for (size_t i = 0; i < itp->variables.count; i++) {
    ItpVar* check_var = vec_get(&itp->variables, i);
    if (str_cmp(&check_var->name, name)) {
      if (str_cstr(name)) return -1;
      if (str_fmt_cstr(&itp->error_string, "Variable \"%s\" already declared", name->text)) return -1;
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
  if (vec_init(&itp->sub_stack, sizeof(size_t))) return -1;

  itp->line_index = 0;
  if (str_init_cstr(&itp->error_string, "Unknown error")) return -1;
  if (str_init(&itp->input_string)) return -1;

  itp->current_line = NULL;
  itp->running = false;

  if (itp_load_constants(itp)) return -1;
  
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
  vec_free(&itp->sub_stack);
  str_free(&itp->error_string);
  str_free(&itp->input_string);
}

int itp_clear(Itp* itp) {
  ASSERT(itp);

  for (size_t i = 0; i < itp->variables.count; i++) {
    ItpVar* var = vec_get(&itp->variables, i);

    _var_free(var);
  }

  vec_clear(&itp->variables);
  vec_clear(&itp->sub_stack);

  if (itp_load_constants(itp)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int itp_load_constants(Itp* itp) {
  ASSERT(itp);

  Str str_const_name;
  if (str_init(&str_const_name)) return _res;

  if (str_cpy_cstr(&str_const_name, "_e")) return _res;
  if (itp_var_add(itp, &str_const_name, &(Token){.type = tt_Float, ._float = M_E})) return -1;

  if (str_cpy_cstr(&str_const_name, "_pi")) return _res;
  if (itp_var_add(itp, &str_const_name, &(Token){.type = tt_Float, ._float = M_PI})) return -1;

  if (str_cpy_cstr(&str_const_name, "_width")) return _res;
  if (itp_var_add(itp, &str_const_name, &(Token){.type = tt_Long, ._long = TERMINAL_WIDTH})) return -1;

  if (str_cpy_cstr(&str_const_name, "_height")) return _res;
  if (itp_var_add(itp, &str_const_name, &(Token){.type = tt_Long, ._long = TERMINAL_HEIGHT})) return -1;

  RES_RETURN(r_ENONE, 0);
}

ItpLine* itp_get_line(Itp* itp, size_t line_num, size_t* index) {
  ASSERT(itp);

  for (size_t i = 0; i <= itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);
    if (line && line->line_num > line_num) RES_RETURN(r_ENONE, NULL);
    else if (line && line->line_num == line_num) {
      if (index) *index = i;
      RES_RETURN(r_ENONE, line);
    }
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

  if (lxr_LexLine(&new_line_ptr->tokens, &itp->error_string, &new_line_ptr->text)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int itp_update(Itp* itp) {
  ASSERT(itp);

  if (term_get_interrupt(itp->term)) {
    if (itp->input_await) itp->input_await = false;
    else if (itp->running) itp->running = false;
    else RES_RETURN(r_INTERRUPTED, 1);
  }

  if (itp->input_await) {
    if (term_get_input(itp->term, itp->input_await_str)) {
      if (_res != r_DATA_READY) return -1;
      itp->input_await = false;
    }
    RES_RETURN(r_ENONE, 0);
  }

  if (itp->running) {
    itp->current_line = vec_get(&itp->lines, itp->line_index);
    if (itp_interpret_line(itp)) {
      result res = _res;

      if (str_cstr(&itp->error_string)) return -1;
      if (term_printf(itp->term, "Error on line %zu:\n%s\n", itp->current_line->line_num, itp->error_string.text)) return -1;
      itp->running = false;

      if (res != r_EINTERPRET) RES_RETURN(res, -1);
      RES_RETURN(r_ENONE, 0);
    }

    if (!itp->dont_jump) itp->line_index++;
    itp->dont_jump = false;

    if (itp->line_index >= itp->lines.count) {
      itp->running = false;
      if (term_printf(itp->term, "Done\n")) return -1;
    }

    RES_RETURN(r_ENONE, 0);
  }

  if (!itp->term->input_await) if (term_printf(itp->term, ">>>")) return -1;
  if (term_get_input(itp->term, &itp->input_string)) {
    if (_res != r_DATA_READY) return -1;
  } else {
    RES_RETURN(r_ENONE, 0);
  }

  ItpLine new_line = {
    .line_num = 0
  };

  if (str_dup(&new_line.text, &itp->input_string)) return -1;
  if (vec_init(&new_line.tokens, sizeof(Token))) return -1;
  if (lxr_LexLine(&new_line.tokens, &itp->error_string, &new_line.text)) {
    result res = _res;

    if (str_cstr(&itp->error_string)) return -1;
    if (term_printf(itp->term, "Lexical analysis failed:\n%s\n", itp->error_string.text)) return -1;

    if (res != r_ELEX) RES_RETURN(res, -1);
    RES_RETURN(r_ENONE, 0);
  }

  Token* line_tok = vec_get(&new_line.tokens, 0);
  if (line_tok && line_tok->type == tt_Long) {
    long line_num = line_tok->_long;
    if (line_num < 0) {
      if (str_cpy_cstr(&itp->error_string, "Cannot have negative line number")) return -1;
      RES_RETURN(r_ENONE, 0);
    }
  
    if (new_line.tokens.count == 1) {
      _line_free(&new_line);
      size_t line_index;
      ItpLine* del_line = itp_get_line(itp, line_num, &line_index);
      if (del_line) {
        _line_free(del_line);
        if (vec_pop_at(&itp->lines, line_index, NULL)) return -1;
      }
      RES_RETURN(r_ENONE, 0);
    }
    if (vec_pop_at(&new_line.tokens, 0, NULL)) return -1;

    for (size_t i = 0; i <= itp->lines.count; i++) {
      ItpLine* line_check = vec_get(&itp->lines, i);

      new_line.line_num = line_num;
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
    itp->current_line = &new_line;
    if (itp_interpret_line(itp)) {
      result res = _res;
      if (str_cstr(&itp->error_string)) return -1;
      if (term_printf(itp->term, "Error during interpreting:\n%s\n", itp->error_string.text)) return -1;
      _line_free(&new_line);

      if (res != r_EINTERPRET) RES_RETURN(res, -1);

      RES_RETURN(r_ENONE, 0);
    }
    _line_free(&new_line);
  }
  RES_RETURN(r_ENONE, 0);
}

int itp_interpret_line(Itp* itp) {
  ASSERT(itp);

  ItpLine* line = itp->current_line;
  if (line->tokens.count == 0) RES_RETURN(r_ENONE, 0);

  Token* token = vec_get(&line->tokens, 0);

  switch (token->type) {
    case tt_Keyword: {
      if (handle_keyword(itp)) {
        if (_res != r_ERETRY) return -1;
        itp->dont_jump = true;
      }
    } break;
    case tt_Variable: {
      if (handle_variable(itp)) return -1;
    } break;
    default: {
      if (str_fmt_cstr(&itp->error_string, "Invalid token here: %s", tok_get_type_cstr(token->type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }
  }
  
  RES_RETURN(r_ENONE, 0);
}

int itp_goto(Itp* itp, size_t line_num) {
  ASSERT(itp);

  ItpLine* line = itp_get_line(itp, line_num, &itp->line_index);
  if (!line) {
    if (str_cpy_cstr(&itp->error_string, "Invalid line number")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  itp->dont_jump = itp->running;
  itp->running = true;

  RES_RETURN(r_ENONE, 0);
}

int itp_gosub(Itp* itp, size_t line_num) {
  ASSERT(itp);

  if (!itp->running) {
    if (str_cpy_cstr(&itp->error_string, "Cannot GOSUB when program is not being run")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }
  
  if (vec_push(&itp->sub_stack, &itp->line_index)) return -1;

  ItpLine* line = itp_get_line(itp, line_num, &itp->line_index);
  if (!line) {
    if (str_cpy_cstr(&itp->error_string, "Invalid line number")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  itp->dont_jump = true;

  RES_RETURN(r_ENONE, 0);
}

int itp_return(Itp* itp) {
  ASSERT(itp);

  if (!itp->running) {
    if (str_cpy_cstr(&itp->error_string, "Cannot RETURN when program is not being run")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (itp->sub_stack.count == 0) {
    if (str_cpy_cstr(&itp->error_string, "Cannot RETURN when not in subroutine")) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (vec_pop(&itp->sub_stack, &itp->line_index)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int itp_print_line(Itp* itp, ItpLine* line) {
  ASSERT(itp);
  ASSERT(line);

  if (str_cstr(&line->text)) return -1;

  printf("Line %zu\n", line->line_num);
  for (size_t i = 0; i < line->tokens.count; i++) {
    Token* token = vec_get(&line->tokens, i);
    printf("(%zu): ", i);
    tok_print(token);
    printf("\n");
  }

  RES_RETURN(r_ENONE, 0);
}

