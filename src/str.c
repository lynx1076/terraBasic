#include "str.h"
#include "result.h"
#include "debug.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int _str_reserve(Str* str, size_t capacity);


int _str_reserve(Str* str, size_t capacity) {
  ASSERT(str);

  if (str->capacity - str->length >= capacity) RES_RETURN(r_ENONE, 0);

  size_t newCap = str->capacity;
  while (newCap < capacity + str->capacity) newCap *= 2;

  char* newBuff = realloc(str->text, newCap);
  if (!newBuff) RES_RETURN(r_EMEM, -1);

  str->text = newBuff;
  str->capacity = newCap;

  RES_RETURN(r_ENONE, 0);
}

int str_init(Str* str) {
  ASSERT(str);

  char* text = malloc(STR_INITIAL_CAPACITY);
  if (!text) RES_RETURN(r_EMEM, -1);

  str->text = text;
  str->capacity = STR_INITIAL_CAPACITY;
  str->length = 0;

  RES_RETURN(r_ENONE, 0);
}

int str_init_cnstr(Str* str, const char* cstr, size_t length) {
  ASSERT(str && cstr);
  
  if (str_init(str)) return -1;
  if (str_cncat(str, cstr, length)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int str_init_cstr(Str* str, const char* cstr) {
  ASSERT(str && cstr);
  
  if (str_init(str)) return -1;
  if (str_ccat(str, cstr)) return -1;

  RES_RETURN(r_ENONE, 0);
}

void str_free(Str* str) {
  ASSERT(str);

  free(str->text);

  *str = STR_EMPTY;
}

int str_cmp(const Str* str, const Str* str2) {
  ASSERT(str && str2);

  if (str->length != str2->length) RES_RETURN(r_ENONE, 0);

  for (size_t i = 0; i < str->length; i++) {
    if (str->text[i] != str2->text[i]) RES_RETURN(r_ENONE, 0);
  }

  RES_RETURN(r_ENONE, 1);
}

int str_cmp_substr(const Str* str, const Str* substr, size_t startIndex) {
  ASSERT(str && substr);
  ASSERT(startIndex >= str->length);

  for (size_t i = 0; i < substr->length; i++) {
    if (str->text[i + startIndex] != substr->text[i]) RES_RETURN(r_ENONE, 0);
  }

  RES_RETURN(r_ENONE, 1);
}

int str_cat(Str* str, const Str* str2) {
  ASSERT(str && str2);

  if (_str_reserve(str, str2->length)) return -1;

  memcpy(str->text + str->length, str2->text, str2->length);
  str->length += str2->length;

  RES_RETURN(r_ENONE, 0);
}

int str_insert(Str* str, const Str* str2, size_t insertIndex) {
  ASSERT(str && str2);
  ASSERT(insertIndex > str->length);

  if (_str_reserve(str, str2->length)) return -1;

  char* start = str->text + insertIndex + str2->length;
  char* src = str->text + insertIndex;
  size_t len = str->length - insertIndex;
  memmove(start, src, len);
  memcpy(str->text + insertIndex, str2->text, str2->length);
  str->length += str2->length;

  RES_RETURN(r_ENONE, 0);
}

int str_cncat(Str* str, const char* cstr2, size_t length) {
  ASSERT(str && cstr2);

  if (_str_reserve(str, length)) return -1;

  memcpy(str->text + str->length, cstr2, length);
  str->length += length;

  RES_RETURN(r_ENONE, 0);
}

int str_ccat(Str* str, const char* cstr2) {
  ASSERT(str && cstr2);

  size_t cstr2Len = strlen(cstr2);

  if (str_cncat(str, cstr2, cstr2Len)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int str_cinsert(Str* str, const char* cstr2, size_t insertIndex) {
  ASSERT(str && cstr2);
  ASSERT(insertIndex <= str->length);

  size_t cstr2Len = strlen(cstr2);

  if (_str_reserve(str, cstr2Len)) return -1;

  memmove(str->text + insertIndex + cstr2Len, str->text + insertIndex, str->length - insertIndex);
  memcpy(str->text + insertIndex, cstr2, cstr2Len);
  str->length += cstr2Len;

  RES_RETURN(r_ENONE, 0);
}

int str_cstr(Str* str) {
  ASSERT(str);

  if (_str_reserve(str, 1)) return -1;

  str->text[str->length] = '\0';

  RES_RETURN(r_ENONE, 0);
}

int str_clear(Str* str) {
  ASSERT(str);

  str->length = 0;

  RES_RETURN(r_ENONE, 0);
}

int str_substr(Str* str, size_t startIndex, size_t length) {
  ASSERT(str);

  str->length -= length + startIndex;
  if (str->length != 0) {
    memmove(str->text, str->text + startIndex, str->length);
  }

  RES_RETURN(r_ENONE, 0);
}

int str_cut(Str* str, size_t startIndex, size_t length) {
  ASSERT(str);
  ASSERT(startIndex + length <= str->length);

  memmove(str->text + startIndex, str->text + startIndex + length, str->length - length - startIndex);
  str->length -= length;

  RES_RETURN(r_ENONE, 0);
}

int str_cpy(Str* copy, const Str* src) {
  ASSERT(src && copy);

  if (str_clear(copy)) return -1;
  if (str_cat(copy, src)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int str_ccpy(Str* copy, const char* src) {
  ASSERT(src && copy);

  if (str_clear(copy)) return -1;
  if (str_ccat(copy, src)) return -1;

  RES_RETURN(r_ENONE, 0);
}

int str_dup(Str* dup, const Str* src) {
  ASSERT(src && dup);

  *dup = *src;

  char* newBuff = malloc(src->capacity);
  if (!newBuff) RES_RETURN(r_EMEM, -1);
  
  dup->text = newBuff;

  RES_RETURN(r_ENONE, 0);
}

int str_replace(Str* str, const Str* strPattern, const Str* strNew) {
  ASSERT(str && strPattern && strNew);

  for (size_t i = 0; i < str->length; i++) {
    if (str_cmp_substr(str, strPattern, i)) {
      if (str_cut(str, i, strPattern->length)) return -1;
      if (str_insert(str, strNew, i)) return -1;
      if (strNew->length > 1) i += strNew->length - 1;
    }
  }

  RES_RETURN(r_ENONE, 0);
}

