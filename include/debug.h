#ifndef DEBUG_H
#define DEBUG_H


#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>

#define ASSERT(BOOL) do { if (!BOOL) { printf("Assert failed on line %i of \"%s\": %s", __LINE__, __FILE__, #BOOL); exit(-1); } } while (0)
#else
#define ASSERT(BOOL) do {} while(0)
#endif

#endif
