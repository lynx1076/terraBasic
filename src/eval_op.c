#include "eval_op.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"
#include <math.h>
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

    if ((rtok.type != tt_Long && rtok.type != tt_Float && rtok.type != tt_String) ||
        (ltok.type != tt_Long && ltok.type != tt_Float && ltok.type != tt_String)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot add %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_str = rtok.type == tt_String;
    bool l_is_str = ltok.type == tt_String;

    Str rstr = rtok.string;
    Str lstr = ltok.string;

    if (r_is_str || l_is_str) {
      if (!r_is_str) {
        if (str_init(&rstr)) return -1;
        if (rtok.type == tt_Long) {
          if (str_fmt_cstr(&rstr, "%li", rtok._long)) return -1;
        } else {
          if (str_fmt_cstr(&rstr, "%f", rtok._float)) return -1;
        }
      }

      if (!l_is_str) {
        if (str_init(&lstr)) return -1;
        if (ltok.type == tt_Long) {
          if (str_fmt_cstr(&lstr, "%li", ltok._long)) return -1;
        } else {
          if (str_fmt_cstr(&lstr, "%f", ltok._float)) return -1;
        }
      }

      if (str_cat(&lstr, &rstr)) return -1;

      res = (Token){
        .type = tt_String,
      };
      if (str_dup(&res.string, &lstr)) return -1;
      if (!r_is_str) str_free(&rstr);
      if (!l_is_str) str_free(&lstr);
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
  case to_Minus: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot subtract %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;
      
    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;

      res = (Token){
        .type = tt_Float,
        ._float = ltok._float - rtok._float
      };
    } else {
      res = (Token){
        .type = tt_Long,
        ._long = ltok._long - rtok._long
      };
    }

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_Mult: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot multiply %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;
      
    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;

      res = (Token){
        .type = tt_Float,
        ._float = ltok._float * rtok._float
      };
    } else {
      res = (Token){
        .type = tt_Long,
        ._long = ltok._long * rtok._long
      };
    }

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_Div: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot multiply %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool make_long = false;

    if (!r_is_float && !l_is_float) {
      make_long = (ltok._long % rtok._long == 0);
    }

    if (make_long) {
      res = (Token){
        .type = tt_Long,
        ._long = ltok._long / rtok._long
      };
    } else {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;

      res = (Token){
        .type = tt_Float,
        ._float = ltok._float / rtok._float
      };
    }

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_Pow: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot multiply %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool make_long = (!r_is_float && !l_is_float) && rtok._long >= 0;

    if (make_long) {
      res = (Token){
        .type = tt_Long,
        ._long = powl(ltok._long, rtok._long)
      };
    } else {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;

      res = (Token){
        .type = tt_Float,
        ._float = powf(ltok._float, rtok._float)
      };
    }

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_Or: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot OR %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float) rtok._long = (long)rtok._float;
    if (l_is_float) ltok._long = (long)ltok._float;

    res = (Token){
      .type = tt_Long,
      ._long = ltok._long | ltok._long
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_And: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float) rtok._long = (long)rtok._float;
    if (l_is_float) ltok._long = (long)ltok._float;

    res = (Token){
      .type = tt_Long,
      ._long = ltok._long & ltok._long
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_Not: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;

    if (rtok.type != tt_Long && rtok.type != tt_Float) {
      if (str_fmt_cstr(&itp->errorString, "Cannot NOT %s", tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;

    if (r_is_float) rtok._long = (long)rtok._float;

    res = (Token){
      .type = tt_Long,
      ._long = ~rtok._long
    };

    tok_free(&rtok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_Eq: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool cmp;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float == rtok._float;
    } else {
      cmp = ltok._long == rtok._long;
    }

    res = (Token){
      .type = tt_Long,
      ._long = cmp ? -1 : 0
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_NEq: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool cmp;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float == rtok._float;
    } else {
      cmp = ltok._long == rtok._long;
    }

    res = (Token){
      .type = tt_Long,
      ._long = cmp ? 0 : -1
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_LT: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool cmp;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float < rtok._float;
    } else {
      cmp = ltok._long < rtok._long;
    }

    res = (Token){
      .type = tt_Long,
      ._long = cmp ? -1 : 0
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_GT: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool cmp;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float > rtok._float;
    } else {
      cmp = ltok._long > rtok._long;
    }

    res = (Token){
      .type = tt_Long,
      ._long = cmp ? -1 : 0
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_LE: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool cmp;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float <= rtok._float;
    } else {
      cmp = ltok._long <= rtok._long;
    }

    res = (Token){
      .type = tt_Long,
      ._long = cmp ? -1 : 0
    };

    tok_free(&rtok);
    tok_free(&ltok);

    if (vec_push(stack, &res)) return -1;
  } break;
  case to_GE: {
    Token res;

    Token rtok;
    if (vec_pop(stack, &rtok)) return -1;
    Token ltok;
    if (vec_pop(stack, &ltok)) return -1;

    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->errorString, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    bool cmp;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float >= rtok._float;
    } else {
      cmp = ltok._long >= rtok._long;
    }

    res = (Token){
      .type = tt_Long,
      ._long = cmp ? -1 : 0
    };

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

