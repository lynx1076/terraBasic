#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>


int strncicmp(const char* str1, const char* str2, size_t length);

int ParseLong(long* value, char* str, size_t length);
int ParseFloat(float* value, const char* str, size_t length);

#endif
