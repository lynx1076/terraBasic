#include "interpreter.h"
#include "interpreter_helpers.h"
#include "debug.h"
#include "lexer.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"
#include <stddef.h>

void _var_free(ItpVar* var);
void _line_free(ItpLine* line);


void _var_free(ItpVar* var) {
  ASSERT(var);
}

void _line_free(ItpLine* line) {
  ASSERT(line);

  for (size_t i = 0; i < line->tokens.count; i++) {
    Token* token = vec_get(&line->tokens, i);

    tok_free(token);
  }
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
}

int itp_line_add(Itp* itp, const Str* text, size_t line_num) {
  ASSERT(itp);

  ItpLine newLine;
  ItpLine* newLinePtr;

  newLine.line_num = line_num;
  
  if (str_dup(&newLine.text, text)) return -1;
  if (vec_init(&newLine.tokens, sizeof(Token))) return -1;

  for (size_t i = 0; i <= itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);

    if (!line || line_num < line->line_num) {
      if (vec_push_to(&itp->lines, i, &newLine)) return -1;
      newLinePtr = vec_get(&itp->lines, i);
      break;
    } else if (line_num == line->line_num) {
      _line_free(line);
      *line = newLine;
      newLinePtr = line;
    }
  }

  if (lxr_LexLine(&newLinePtr->tokens, &itp->errorString, &newLinePtr->text)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int itp_interpret_line(Itp* itp, size_t i) {
  ASSERT(itp);

  ItpLine* line = vec_get(&itp->lines, i);
  Token* token = vec_get(&line->tokens, 0);

  switch (token->type) {
    case tt_Identifier: {
      if (handle_variable(itp)) return -1;
    } break;
    case tt_Keyword: {
      if (handle_keyword(itp)) return -1;
    } break;
    default: break;
  }
  
  RES_RETURN(r_ENONE, 0);
}

int itp_print_line(Itp* itp, size_t line_num) {
  ASSERT(itp);

  for (size_t i = 0; i < itp->lines.count; i++) {
    ItpLine* line = vec_get(&itp->lines, i);
    if (line->line_num != line_num) continue;

    printf("%zu:\n", line_num);
    if (str_cstr(&line->text)) return -1;
    printf("%s\n", line->text.text);

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

