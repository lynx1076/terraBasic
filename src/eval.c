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


int evl_eval(Itp* itp, size_t start) {
  ASSERT(itp);

  Vec rpn;
  if (vec_init(&rpn, sizeof(Token))) return -1;

  if (_evl_get_rpn(itp, start, &rpn)) return -1;

  Vec stack;
  if (vec_init(&stack, sizeof(Token))) return -1;

  printf("RPN:\n");
  for (size_t i = 0; i < rpn.count; i++) {
    Token* token = vec_get(&rpn, i);
    printf("[%zu] ", i);
    tok_print(token);
    printf("\n");
  }

  RES_RETURN(r_ENONE, 0);
}

int _evl_get_rpn(Itp* itp, size_t start, Vec* rpn) {
  ASSERT(itp);

  size_t i = start;

  Vec* tokens = &((ItpLine*)vec_get(&itp->lines, itp->lineIndex))->tokens;
  Token* token = vec_get(tokens, i);

  Vec stack;
  if (vec_init(&stack, sizeof(Token))) return -1;

  size_t functionDepth = 0;

  while (token) {
    printf("Token[%zu] ", i);
    tok_print(token);
    printf("\n");
    switch (token->type) {
      case tt_Function: {
        Token* nextTok = vec_get(tokens, i);
        if (!nextTok || nextTok->type != tt_Operator || nextTok->operator != to_LParen) {
          if (str_cpy_cstr(&itp->errorString, "Expected parenthesis after function declaration")) return -1;
          RES_RETURN(r_EINTERPRET, -1);
        }
        functionDepth++;
        if (vec_push(&stack, token)) return -1;
      } break;
      case tt_Identifier:
      case tt_Long:
      case tt_Float: {
        if (vec_push(rpn, token)) return -1;
      } break;
      case tt_Operator: {
        Token* stack_token = vec_get_last(&stack);
        TokenOperator stack_operator;
        if (stack_token && stack_token->type == tt_Operator) stack_operator = stack_token->operator;
        else stack_operator = to_None;
        TokenOperator token_operator = token->operator;

        size_t stack_precedence = tok_get_operator_precedence(stack_operator);
        size_t token_precedence = tok_get_operator_precedence(token_operator);

        if (token->operator == to_RParen) {
          bool noOpen = false;
          while (stack_token->operator != to_LParen) {
            if (vec_push(rpn, stack_token)) return -1;
            vec_pop(&stack, NULL);
            if (stack.count) {
              stack_token = vec_get_last(&stack);
            } else {
              noOpen = true;
              break;
            }
          }

          if (noOpen) {
            if (str_cpy_cstr(&itp->errorString, "Missing opening parenthesis")) return -1;
            RES_RETURN(r_ENONE, -1);
          }
          
          if (vec_pop(&stack, NULL)) return -1;
          
        } else if (token->operator == to_LParen) {
          if (vec_push(&stack, token)) return -1;
        } else if (stack_precedence > token_precedence) {
          Token tmpTok;

          if (vec_pop(&stack, &tmpTok)) return -1;
          if (vec_push(rpn, &tmpTok)) return -1;
          if (vec_push(&stack, token)) return -1;

          printf("Switch out ");
          tok_print(token);
          printf(" with ");
          tok_print(&tmpTok);
          printf("\n");
        } else {
          if (vec_push(&stack, token)) return -1;
        }
      } break;
      default: {
        if (str_fmt_cstr(&itp->errorString, "%s not allowed here", tok_get_type_cstr(token->type))) return -1;
      } RES_RETURN(r_EINTERPRET, -1);
    }

    printf("Stack:\n");
    for (size_t i = 0; i < stack.count; i++) {
      printf("[%zu]: ", i);
      tok_print(vec_get(&stack, i));
      printf("\n");
    }

    printf("Queue:\n");
    for (size_t i = 0; i < rpn->count; i++) {
      printf("[%zu]: ", i);
      tok_print(vec_get(rpn, i));
      printf("\n");
    }
    printf("\n");

    token = vec_get(tokens, ++i);
  }

  while (stack.count) {
    Token tok;
    vec_pop(&stack, &tok);
    if (vec_push(rpn, &tok)) return -1;
    if (tok.operator == to_LParen) {
      if (str_cpy_cstr(&itp->errorString, "Missing closing parenthesis")) return -1;
    }
  }

  RES_RETURN(r_ENONE, 0);
}

