#ifndef BUILTINS_H
#define BUILTINS_H

int bin_StrLeft();
int bin_StrRight();
int bin_StrMiddle();

#define BUILTINS \
  X(bin_StrLeft) \
  X(bin_StrRight) \
  X(bin_StrMiddle) \

#endif
