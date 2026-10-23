#include "interpreter.h"
#include "debug.h"
#include "lexer.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"
#include <stddef.h>

void _var_free(ItpVar* var);
void _line_free(ItpLine* line);
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
  ASSERT(var);
}

void _line_free(ItpLine* line) {
  ASSERT(line);

  for (size_t i = 0; i < line->tokens.count; i++) {
    Token* token = vec_get(&line->tokens, i);

    tok_free(token);
  }
}

int _interpret_keyword(Itp* itp) {
  
}

int _interpret_identifier(Itp* itp) {
  ASSERT(itp);

  ItpLine* line = vec_get(&itp->lines, itp->lineIndex);
  Token* first_tok = vec_get(&line->tokens, 0);

  ItpVar* var = _get_var(itp, &first_tok->string);
  if (!var) {
    if (str_cstr(&first_tok->string)) return -1;
    str_fmt_cstr(&itp->errorString, "Not a valid identifier or keyword: \"%s\"", first_tok->string); 
  }

  RES_RETURN(r_ENONE, 0);
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
    str_cpy_cstr(&itp->errorString, "Bad.");
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

