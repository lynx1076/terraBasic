#include "eval.h"
#include "debug.h"
#include "result.h"
#include "interpreter.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>


int _evl_get_rpn(Itp* itp, size_t start, Vec* rpn);


int evl_eval_float(Itp* itp, size_t start) {
  ASSERT(itp);

  printf("Evaluate\n");

  Vec rpn;
  if (_evl_get_rpn(itp, start, &rpn)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int _evl_get_rpn(Itp* itp, size_t start, Vec* rpn) {
  ASSERT(itp);

  size_t i = start;

  Vec* tokens = &((ItpLine*)vec_get(&itp->lines, itp->lineIndex))->tokens;
  Token* token = vec_get(tokens, i);

  Vec stack;
  if (vec_init(&stack, sizeof(Token))) return -1;
  Vec queue;
  if (vec_init(&queue, sizeof(Token))) return -1;

  while (token) {
    switch (token->type) {
    case tt_Long:
    case tt_Float: {
      if (vec_push(&queue, token)) return -1;
      printf("Add operand\n");
    } break;
    case tt_Operator: {
      Token* stackToken = vec_get_last(&stack);
      TokenOperator stack_operator;
      if (stackToken) stack_operator = stackToken->operator;
      else stack_operator = to_None;
      TokenOperator token_operator = token->operator;

      size_t stack_precedence = tok_get_operator_precedence(stack_operator);
      size_t token_precedence = tok_get_operator_precedence(token_operator);

      printf("Stack size: %zu\n", stack.count);
      printf("Stack: %zu, op: %s\n", stack_precedence, tok_get_operator_cstr(stack_operator));
      printf("Token: %zu, op: %s\n", token_precedence, tok_get_operator_cstr(token_operator));

      if (token->operator == to_RParen) {
        while (stackToken && stackToken->operator != to_LParen) {
          printf("Paren token ");
          tok_print(stackToken);
          printf("\n");
          
          vec_pop(&stack, &stackToken);
          if (vec_push(&queue, &stackToken)) return -1;
          stackToken = vec_get_last(&stack);
        }
        vec_pop(&stack, &stackToken);
        if (stack.count == 0 && stackToken->operator != to_LParen) {
          if (str_cpy_cstr(&itp->errorString, "Missing opening parenthesis")) return -1;
        }
      } else if (stack_precedence > token_precedence) {
        TokenOperator tmpop = token_operator;
        token_operator = stack_operator;
        stack_operator = tmpop;

        if (vec_pop(&stack, stackToken)) return -1;
        if (vec_push(&queue, stackToken)) return -1;
        if (vec_push(&stack, token)) return -1;
        printf("Switch out ");
        tok_print(stackToken);
        printf(" with ");
        tok_print(token);
        printf("\n");
      } else {
        if (vec_push(&stack, token)) return -1;
        printf("Add ");
        tok_print(token);
        printf("\n");
      }
    } break;

    default: {

    } RES_RETURN(r_EINTERPRET, -1);
    }

    printf("\n");

    token = vec_get(tokens, ++i);
  }

  while (stack.count) {
    Token tok;
    vec_pop(&stack, &tok);
    if (vec_push(&queue, &tok)) return -1;
    if (tok.operator == to_LParen) {
      if (str_cpy_cstr(&itp->errorString, "Missing closing parenthesis")) return -1;
    }
  }

  for (size_t i = 0; i < queue.count; i++) {
    Token* token = vec_get(&queue, i);
    printf("[%zu] ", i);
    tok_print(token);
    printf("\n");
  }

  RES_RETURN(r_ENONE, 0);
}

