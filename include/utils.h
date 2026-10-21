#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include <stddef.h>


#define DEG_TO_RAD(DEG)         (DEG * PI / 180)
#define RAD_TO_DEG(RAD)         (RAD * 180 / PI)

#define VEC_ADD(A, B)           ((Vector2){(A).x + (B).x, (A).y + (B).y})
#define VEC_SUB(A, B)           ((Vector2){(A).x - (B).x, (A).y - (B).y})
#define VEC_MULT(A, X)          ((Vector2){(A).x * (X), (A).y * (X)})


float GetRandomFloat();
float GetRandomFloatRange(float min, float max);
float Clampf(float min, float max, float x);
float Lerpf(float a, float b, float x);
Vector2 LerpV2(Vector2 a, Vector2 b, float x);

void PrintVector(Vector2 v);

int strncicmp(const char* str1, const char* str2, size_t length);

#endif
