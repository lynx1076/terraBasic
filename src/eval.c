#include "eval.h"
#include "eval_func.h"
#include "eval_op.h"
#include "debug.h"
#include "interpreter_helpers.h"
#include "result.h"
#include "interpreter.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>


int _eval_rpn(Itp* itp, Vec* rpn, Token* res);
int _get_rpn(Itp* itp, size_t start, size_t stop, Vec* rpn);


int evl_eval(Itp* itp, size_t start, size_t stop, Token* res) {
  ASSERT(itp);

  Vec rpn;
  if (vec_init(&rpn, sizeof(Token))) return -1;

  if (_get_rpn(itp, start, stop, &rpn)) return -1;

  if (_eval_rpn(itp, &rpn, res)) return -1;

  for (size_t i = 0; i < rpn.count; i++) {
    Token* tok = vec_get(&rpn, i);
    tok_free(tok);
  }

  RES_RETURN(r_ENONE, 0);
}

int _eval_rpn(Itp* itp, Vec* rpn, Token* res) {
  ASSERT(itp);
  ASSERT(rpn);

  Vec stack;
  if (vec_init(&stack, sizeof(Token))) return -1;

  for (size_t i = 0; i < rpn->count; i++) {
    Token* token = vec_get(rpn, i);
    Token cpy;

    if (token->type == tt_Operator) {
      if (evl_eval_op(itp, token, &stack)) return -1;
    } else if (token->type == tt_Function) {
      if (evl_eval_func(itp, token, &stack)) return -1;
    } else if (token->type == tt_Variable) {
      Token* var = &itp_get_var(itp, &token->string)->token;
      if (tok_duplicate(&cpy, var)) return -1;
      if (vec_push(&stack, &cpy)) return -1;
    } else {
      if (tok_duplicate(&cpy, token)) return -1;
      if (vec_push(&stack, &cpy)) return -1;
    }
  }

  if (vec_pop(&stack, res)) return -1;
  ASSERT(stack.count == 0);
  vec_free(&stack);

  RES_RETURN(r_ENONE, 0);
}

int _change_sign(const Token* prev) {
  ASSERT(prev);

  switch (prev->operator) {
    case to_LParen:
    case to_Or:
    case to_And:
    case to_Not:
    case to_Eq:
    case to_NEq:
    case to_LT:
    case to_GT:
    case to_LE:
    case to_GE:
      return 1;
    default:
      return 0;
  }
}

int _get_rpn(Itp* itp, size_t start, size_t stop, Vec* rpn) {
  ASSERT(itp);
  ASSERT(rpn);

  size_t i = start;

  Vec* tokens = &((ItpLine*)vec_get(&itp->lines, itp->lineIndex))->tokens;
  Token* token = vec_get(tokens, i);
  Token* last_token = NULL;

  Vec stack;
  if (vec_init(&stack, sizeof(Token))) return -1;

  size_t function_depth = 0;

  while (token && i < stop) {
    switch (token->type) {
      case tt_Function: {
        Token* nextTok = vec_get(tokens, i + 1);
        if (!nextTok || nextTok->type != tt_Operator || nextTok->operator != to_LParen) {
          if (str_cpy_cstr(&itp->errorString, "Expected parenthesis after function declaration")) return -1;
          RES_RETURN(r_EINTERPRET, -1);
        }
        function_depth++;
        token->func.argCount = 1;
        if (vec_push(&stack, token)) return -1;
      } break;
      case tt_Variable:
      case tt_Long:
      case tt_String:
      case tt_Float: {
        if (vec_push(rpn, token)) return -1;
      } break;
      case tt_Operator: {
        if (token->operator == to_Minus) {
          if (!last_token || _change_sign(last_token)) {
            Token zero_tok = {.type = tt_Long, ._long = 0}; // Sketchy :(
            if (vec_push(rpn, &zero_tok)) return -1;
          }
        }

        Token* stack_token = vec_get_last(&stack);
        TokenOperator stack_operator;
        if (stack_token && stack_token->type == tt_Operator) stack_operator = stack_token->operator;
        else stack_operator = to_None;
        TokenOperator token_operator = token->operator;

        size_t stack_precedence = tok_get_operator_precedence(stack_operator);
        size_t token_precedence = tok_get_operator_precedence(token_operator);

        if (token->operator == to_RParen) {
          bool no_open = false;
          while (stack_token->operator != to_LParen) {
            if (vec_push(rpn, stack_token)) return -1;
            if (vec_pop(&stack, NULL)) return -1;
            if (stack.count) {
              stack_token = vec_get_last(&stack);
            } else {
              no_open = true;
              break;
            }
          }

          if (no_open) {
            if (str_cpy_cstr(&itp->errorString, "Missing opening parenthesis")) return -1;
            RES_RETURN(r_ENONE, -1);
          }
          
          if (vec_pop(&stack, NULL)) return -1;
          if (function_depth) {
            function_depth--;
           
            Token* funcToken = vec_get_last(&stack);
            if (vec_push(rpn, funcToken)) return -1;
            if (vec_pop(&stack, NULL)) return -1;
          }
        } else if (token->operator == to_LParen) {
          if (vec_push(&stack, token)) return -1;
        } else if (token->operator == to_Comma) {
          if (function_depth == 0) {
            if (str_cpy_cstr(&itp->errorString, "Cannot have comma outside function parameter list")) return -1;
            RES_RETURN(r_EINTERPRET, -1);
          }

          while (stack_token->operator != to_LParen) {
            if (vec_push(rpn, stack_token)) return -1;
            if (vec_pop(&stack, NULL)) return -1;
            
            stack_token = vec_get_last(&stack);
          }

          Token* last_func = vec_get_last(&stack);
          for (size_t i = 0; last_func->type != tt_Function; i++) {
            last_func = vec_get(&stack, i);
          }
          last_func->func.argCount++;
        } else if (stack_precedence > token_precedence) {
          Token tmpTok;

          if (vec_pop(&stack, &tmpTok)) return -1;
          if (vec_push(rpn, &tmpTok)) return -1;
          if (vec_push(&stack, token)) return -1;

        } else {
          if (vec_push(&stack, token)) return -1;
        }
      } break;
      default: {
        if (str_fmt_cstr(&itp->errorString, "%s not allowed here", tok_get_type_cstr(token->type))) return -1;
      } RES_RETURN(r_EINTERPRET, -1);
    }

    last_token = token;
    token = vec_get(tokens, ++i);
  }

  while (stack.count) {
    Token tok;
    vec_pop(&stack, &tok);
    if (vec_push(rpn, &tok)) return -1;
    if (tok.operator == to_LParen) {
      if (str_cpy_cstr(&itp->errorString, "Missing closing parenthesis")) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }
  }

  RES_RETURN(r_ENONE, 0);
}

