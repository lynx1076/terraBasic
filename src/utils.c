#include "utils.h"
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

