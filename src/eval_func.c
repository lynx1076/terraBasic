#include "eval_func.h"
#include "debug.h"
#include "result.h"
#include "str.h"
#include "tokens.h"
#include <math.h>
#include <stddef.h>
#include <stdlib.h>


int evl_eval_func(Itp* itp, Token* token, Vec* stack) {
  ASSERT(itp);
  ASSERT(token);
  ASSERT(stack);
  ASSERT(token->type == tt_Function);

  switch (token->func.func) {
#define X(TOK_STR, TOK_ENUM, TOK_EVAL) case TOK_ENUM: return TOK_EVAL(itp, stack, token);
    TOK_FUNCTIONS
#undef X
  default:
    RES_RETURN(r_EUNREACHABLE, -1);
  }

  RES_RETURN(r_ENONE, 0);
}

int eval_sin(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (!r_is_float) rtok._float = (float)rtok._long;

  res = (Token){
    .type = tt_Float,
    ._float = sinf(rtok._float)
  };

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_cos(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (!r_is_float) rtok._float = (float)rtok._long;

  res = (Token){
    .type = tt_Float,
    ._float = cosf(rtok._float)
  };

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_tan(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (!r_is_float) rtok._float = (float)rtok._long;

  res = (Token){
    .type = tt_Float,
    ._float = tanf(rtok._float)
  };

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_asin(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (!r_is_float) rtok._float = (float)rtok._long;

  res = (Token){
    .type = tt_Float,
    ._float = asinf(rtok._float)
  };

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_acos(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (!r_is_float) rtok._float = (float)rtok._long;

  res = (Token){
    .type = tt_Float,
    ._float = acosf(rtok._float)
  };

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_atan(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (!r_is_float) rtok._float = (float)rtok._long;

  res = (Token){
    .type = tt_Float,
    ._float = atanf(rtok._float)
  };

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_abs(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 1);

  if (token->func.argCount != 1) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token rtok;
  if (vec_pop(stack, &rtok)) return -1;

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s of %s", tok_function_get_cstr(token->func.func), tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (rtok.type != tt_Long && rtok.type != tt_Float) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get abs of %s", tok_get_type_cstr(rtok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  bool r_is_float = rtok.type == tt_Float;
  
  if (r_is_float) {
    res = (Token){
      .type = tt_Float,
      ._float = fabsf(rtok._float)
    };
  } else {
    res = (Token){
      .type = tt_Long,
      ._long = labs(rtok._long)
    };
  }

  tok_free(&rtok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_left(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 2);

  if (token->func.argCount != 2) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token numTok;
  if (vec_pop(stack, &numTok)) return -1;

  Token str_tok;
  if (vec_pop(stack, &str_tok)) return -1;

  if (numTok.type != tt_Long) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need int", tok_function_get_cstr(token->func.func), tok_get_type_cstr(numTok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (str_tok.type != tt_String) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need string", tok_function_get_cstr(token->func.func), tok_get_type_cstr(str_tok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if ((size_t)numTok._long > str_tok.string.length) numTok._long = str_tok.string.length;

  if (str_substr(&str_tok.string, 0, numTok._long)) return -1;

  res.type = tt_String;
  if (str_dup(&res.string, &str_tok.string)) return -1;

  tok_free(&numTok);
  tok_free(&str_tok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_right(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= 2);

  if (token->func.argCount != 2) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token res;

  Token numTok;
  if (vec_pop(stack, &numTok)) return -1;

  Token str_tok;
  if (vec_pop(stack, &str_tok)) return -1;

  if (numTok.type != tt_Long) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need int", tok_function_get_cstr(token->func.func), tok_get_type_cstr(numTok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (str_tok.type != tt_String) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need string", tok_function_get_cstr(token->func.func), tok_get_type_cstr(str_tok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if ((size_t)numTok._long > str_tok.string.length) numTok._long = str_tok.string.length;

  if (str_substr(&str_tok.string, str_tok.string.length - numTok._long, numTok._long)) return -1;

  res.type = tt_String;
  if (str_dup(&res.string, &str_tok.string)) return -1;

  tok_free(&numTok);
  tok_free(&str_tok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

int eval_mid(Itp* itp, Vec* stack, Token* token) {
  ASSERT(itp);
  ASSERT(stack);

  ASSERT(stack->count >= token->func.argCount);

  Token lenTok;
  bool len_defined = false;

  size_t len;

  if (token->func.argCount == 3) {
    if (vec_pop(stack, &lenTok)) return -1;
    len_defined = true;
    
    if (lenTok.type == tt_Long) {
      len = lenTok._long;
    } else {
      if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need int", tok_function_get_cstr(token->func.func), tok_get_type_cstr(lenTok.type))) return -1;
      RES_RETURN(r_EINTERPRET, -1);
    }
  } else if (token->func.argCount != 2) {
    if (str_fmt_cstr(&itp->error_string, "Invalid arg count for %s", tok_function_get_cstr(token->func.func))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token start_tok;
  if (vec_pop(stack, &start_tok)) return -1;
  if (start_tok.type != tt_Long) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need int",
        tok_function_get_cstr(token->func.func), tok_get_type_cstr(start_tok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  Token str_tok;
  if (vec_pop(stack, &str_tok)) return -1;
  if (str_tok.type != tt_String) {
    if (str_fmt_cstr(&itp->error_string, "Cannot get %s with %s, need string",
        tok_function_get_cstr(token->func.func), tok_get_type_cstr(str_tok.type))) return -1;
    RES_RETURN(r_EINTERPRET, -1);
  }

  if (!len_defined) {
    len = str_tok.string.length;
  }

  Token res;

  if (str_substr(&str_tok.string, start_tok._long, len)) return -1;

  res.type = tt_String;
  if (str_dup(&res.string, &str_tok.string)) return -1;

  tok_free(&str_tok);
  tok_free(&start_tok);

  if (vec_push(stack, &res)) return -1;
 
  RES_RETURN(r_ENONE, 0);
}

