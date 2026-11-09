#include "eval_op.h"
#include "debug.h"
#include "interpreter.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include "vector.h"
#include <math.h>
#include <stddef.h>


int evl_eval_op(Itp* itp, Token* token, Vec* stack) {
  ASSERT(itp);
  ASSERT(token);
  ASSERT(stack);
  ASSERT(token->type == tt_Operator);

  switch (token->operator) {
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_EVAL(itp, stack);
    TOK_OPS
#undef X
  default:
    RES_RETURN(r_ENOT_IMPLEMENTED, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int eval_none(Itp* itp, Vec* stack) {
  (void)itp;
  (void)stack;

  printf("Tried to evaluate operator that should not be evaluated here!\n");
  ASSERT(0);
}

int eval_plus(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float && rtok.type != tt_String) ||
      (ltok.type != tt_Long && ltok.type != tt_Float && ltok.type != tt_String)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot add %s and %s", 
        tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_str = rtok.type == tt_String;
  bool l_is_str = ltok.type == tt_String;

  if (r_is_str || l_is_str) {
    if (str_init(&res.string)) return -1;
    
    if (r_is_str && l_is_str) {
      if (str_cat(&res.string, &ltok.string)) return -1;
      if (str_cat(&res.string, &rtok.string)) return -1;
    } else if (!r_is_str) {
      if (rtok.type == tt_Long) {
        if (str_fmt_cstr(&res.string, "%li", rtok._long)) return -1;
      } else {
        if (str_fmt_cstr(&res.string, "%f", rtok._float)) return -1;
      }
      if (str_insert(&res.string, &ltok.string, 0)) return -1;
    } else if (!l_is_str) {
      if (ltok.type == tt_Long) {
        if (str_fmt_cstr(&res.string, "%li", ltok._long)) return -1;
      } else {
        if (str_fmt_cstr(&res.string, "%f", ltok._float)) return -1;
      }
      if (str_cat(&res.string, &rtok.string)) return -1;
    }

    res.type = tt_String;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_minus(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
      (ltok.type != tt_Long && ltok.type != tt_Float)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot subtract %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_mult(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
      (ltok.type != tt_Long && ltok.type != tt_Float)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot multiply %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_div(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
      (ltok.type != tt_Long && ltok.type != tt_Float)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot multiply %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_pow(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
      (ltok.type != tt_Long && ltok.type != tt_Float)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot multiply %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_or(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
      (ltok.type != tt_Long && ltok.type != tt_Float)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot OR %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_and(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
      (ltok.type != tt_Long && ltok.type != tt_Float)) {
    if (str_fmt_cstr(&itp->error_string, "Cannot AND %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_not(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 1);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot NOT %s", tok_get_type_cstr(rtok.type))) return -1;
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

  RES_RETURN(r_ENONE, 0);
}

int eval_eq(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  bool cmp;

  if (rtok.type == ltok.type && rtok.type == tt_String) {
    cmp = str_cmp(&rtok.string, &ltok.string);
  } else {
    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->error_string, "Cannot compare %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float == rtok._float;
    } else {
      cmp = ltok._long == rtok._long;
    }
  }

  res = (Token){
    .type = tt_Long,
    ._long = cmp ? -1 : 0
  };

  tok_free(&rtok);
  tok_free(&ltok);

  if (vec_push(stack, &res)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int eval_neq(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  bool cmp;

  if (rtok.type == ltok.type && rtok.type == tt_String) {
    cmp = str_cmp(&rtok.string, &ltok.string);
  } else {
    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->error_string, "Cannot compare %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float == rtok._float;
    } else {
      cmp = ltok._long == rtok._long;
    }
  }

  res = (Token){
    .type = tt_Long,
    ._long = cmp ? 0 : -1
  };

  tok_free(&rtok);
  tok_free(&ltok);

  if (vec_push(stack, &res)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int eval_lt(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  bool cmp;

  if (rtok.type == ltok.type && rtok.type == tt_String) {
    cmp = str_is_less(&ltok.string, &rtok.string);
  } else {
    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->error_string, "Cannot compare %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float < rtok._float;
    } else {
      cmp = ltok._long < rtok._long;
    }
  }

  res = (Token){
    .type = tt_Long,
    ._long = cmp ? -1 : 0
  };

  tok_free(&rtok);
  tok_free(&ltok);

  if (vec_push(stack, &res)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int eval_gt(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  bool cmp;

  if (rtok.type == ltok.type && rtok.type == tt_String) {
    cmp = str_is_greater(&ltok.string, &rtok.string);
  } else {
    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->error_string, "Cannot compare %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float > rtok._float;
    } else {
      cmp = ltok._long > rtok._long;
    }
  }

  res = (Token){
    .type = tt_Long,
    ._long = cmp ? -1 : 0
  };

  tok_free(&rtok);
  tok_free(&ltok);

  if (vec_push(stack, &res)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int eval_le(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  bool cmp;

  if (rtok.type == ltok.type && rtok.type == tt_String) {
    cmp = str_is_less(&ltok.string, &rtok.string);
    if (str_cmp(&ltok.string, &rtok.string)) cmp = true;
  } else {
    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->error_string, "Cannot compare %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float < rtok._float;
    } else {
      cmp = ltok._long < rtok._long;
    }
  }

  res = (Token){
    .type = tt_Long,
    ._long = cmp ? -1 : 0
  };

  tok_free(&rtok);
  tok_free(&ltok);

  if (vec_push(stack, &res)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int eval_ge(Itp* itp, Vec* stack) {
  ASSERT(stack->count >= 2);
  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;
  Token ltok;
  if (vec_pop(stack, &ltok)) return -1;

  bool cmp;

  if (rtok.type == ltok.type && rtok.type == tt_String) {
    cmp = str_is_greater(&ltok.string, &rtok.string);
    if (str_cmp(&ltok.string, &rtok.string)) cmp = true;
  } else {
    if ((rtok.type != tt_Long && rtok.type != tt_Float) ||
        (ltok.type != tt_Long && ltok.type != tt_Float)) {
      if (str_fmt_cstr(&itp->error_string, "Cannot compare %s and %s", tok_get_type_cstr(ltok.type), tok_get_type_cstr(rtok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }

    bool r_is_float = rtok.type == tt_Float;
    bool l_is_float = ltok.type == tt_Float;

    if (r_is_float || l_is_float) {
      if (!r_is_float) rtok._float = (float)rtok._long;
      if (!l_is_float) ltok._float = (float)ltok._long;
      cmp = ltok._float > rtok._float;
    } else {
      cmp = ltok._long > rtok._long;
    }
  }

  res = (Token){
    .type = tt_Long,
    ._long = cmp ? -1 : 0
  };

  tok_free(&rtok);
  tok_free(&ltok);

  if (vec_push(stack, &res)) return -1;

  RES_RETURN(r_ENONE, 0);
}

