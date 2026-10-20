#ifndef STR_H
#define STR_H

#include <stddef.h>

#define STR_INITIAL_CAPACITY    5
#define STR_EMPTY (Str){ .text = NULL, .capacity = 0, .length = 0 }

typedef struct {
  char* text;
  size_t capacity;
  size_t length;
} Str;

int str_init(Str* str);
int str_init_cnstr(Str* str, const char* cstr, size_t length);
int str_init_cstr(Str* str, const char* cstr);
void str_free(Str* str);

int str_cmp(const Str* str, const Str* str2);
int str_cmp_substr(const Str* str, const Str* substr, size_t startIndex);

int str_cat(Str* str, const Str* str2);
int str_insert(Str* str, const Str* str2, size_t insertIndex);

int str_cncat(Str* str, const char* cstr2, size_t length);
int str_ccat(Str* str, const char* cstr2);
int str_cinsert(Str* str, const char* cstr2, size_t insertIndex);
int str_cstr(Str* str);

int str_clear(Str* str);
int str_substr(Str* str, size_t startIndex, size_t length);
int str_cut(Str* str, size_t startIndex, size_t length);
int str_cpy(Str* copy, const Str* src);
int str_ccpy(Str* copy, const char* src);
int str_dup(Str* dup, const Str* str);
int str_replace(Str* str, const Str* strPattern, const Str* strNew);

#endif
