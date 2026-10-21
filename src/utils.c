#include "utils.h"
#include "debug.h"
#include <ctype.h>
#include <raylib.h>
#include <stdio.h>

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

