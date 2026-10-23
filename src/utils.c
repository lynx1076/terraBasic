#include "utils.h"
#include "debug.h"
#include "result.h"
#include <ctype.h>
#include <raylib.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


float GetRandomFloat() {
  return (float)GetRandomValue(0, 1000000) / 1000000.0f;
}

float GetRandomFloatRange(float min, float max) {
  return min + ((float)GetRandomValue(0, 1000000) / 1000000.0f) * (max - min);
}

float Clampf(float min, float max, float x) {
  if (x <= min) return min;
  if (x >= max) return max;
  return x;
}

float Lerpf(float a, float b, float x) {
  x = Clampf(0, 1, x);
  return a - (a - b) * x;
}

Vector2 LerpV2(Vector2 a, Vector2 b, float x) {
  return (Vector2){Lerpf(a.x, b.x, x), Lerpf(a.y, b.y, x)};
}

void PrintVector(Vector2 v) {
  printf("%f, %f", v.x, v.y);
}

int strncicmp(const char* str1, const char* str2, size_t length) {
  ASSERT(str1);
  ASSERT(str2);

  while (tolower(*str1) == tolower(*str2) && length) {
    length--;

    str1++;
    str2++;
  }

  if (!length) return 0;
  if (*str1 > *str2) return 1;
  return -1;
}

int ParseLong(long* value, char* str, size_t length) {
  ASSERT(str);
  ASSERT(value);

  if (length == 0) RES_RETURN(r_EPARSE, -1);

  char buf[length + 1];
  snprintf(buf, sizeof(buf), "%.*s", (int)length, str);

  errno = 0;
  char* end;
  long val = strtol(buf, &end, 10);

  if (end != buf + length) {
      RES_RETURN(r_EPARSE, -1);
  }

  if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
    RES_RETURN(r_EPARSE, -1);
  }

  if (val > INT_MAX || val < INT_MIN) {
    RES_RETURN(r_EPARSE, -1);
  }

  *value = (int)val;
  RES_RETURN(r_ENONE, 0);
}

int ParseFloat(float* value, const char* str, size_t length) {
  ASSERT(str);
  ASSERT(value);

  if (length == 0) RES_RETURN(r_EPARSE, -1);

  char buf[length + 1];
  snprintf(buf, sizeof(buf), "%.*s", (int)length, str);

  errno = 0;
  char* end;
  
  float val = strtof(buf, &end);

  if (end != buf + length) {
    RES_RETURN(r_EPARSE, -1);
  }

  if (errno == ERANGE) {
    RES_RETURN(r_EPARSE, -1);
  }

  *value = val;
  RES_RETURN(r_ENONE, 0);
}

