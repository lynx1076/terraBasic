#include "eval_op.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"
#include <stdio.h>


int evl_eval_op(Itp* itp, Token* token, Vec* stack) {
  ASSERT(itp);
  ASSERT(token);
  ASSERT(stack);
  ASSERT(token->type == tt_Operator);

  switch (token->operator) {
  case to_Plus: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if (rtok.type != tt_Long && rtok.type != tt_Float && rtok.type != tt_String) {
      if (str_fmt_cstr(&itp->errorString, "Cannot add %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_str = rtok.type == tt_String;
    bool l_is_str = ltok.type == tt_String;

    if (r_is_str || l_is_str) {
      Str rstr = rtok.string;
      Str lstr = ltok.string;

      if (!r_is_str) {
        if (rtok.type == tt_Long) {
          if (str_fmt_cstr(&rstr, "%li", rtok._long)) return -1;
        } else {
          if (str_fmt_cstr(&rstr, "%f", rtok._float)) return -1;
        }
      }

      if (!l_is_str) {
        if (ltok.type == tt_Long) {
          if (str_fmt_cstr(&lstr, "%li", ltok._long)) return -1;
        } else {
          if (str_fmt_cstr(&lstr, "%f", ltok._float)) return -1;
        }
      }

      if (str_cat(&lstr, &rstr)) return -1;

      res = (Token){
        .type = tt_String,
        .string = lstr
      };
    } else {
      bool r_is_float = rtok.type == tt_Float;
      bool l_is_float = ltok.type == tt_Float;
      
      if (r_is_float || l_is_float) {
        if (!r_is_float) rtok._float = (float)rtok._long;
        if (!l_is_float) ltok._float = (float)ltok._long;

        res = (Token){
          .type = tt_Float,
          ._float = rtok._float + ltok._float
        };
      } else {
        res = (Token){
          .type = tt_Long,
          ._long = rtok._long + ltok._long
        };
      }
    }

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  default:
    printf("Cannot evaluate %s yet\n", tok_operator_get_cstr(token->operator));
    RES_RETURN(r_ENOT_IMPLEMENTED, -1);
  }

  RES_RETURN(r_ENONE, 0);
}


