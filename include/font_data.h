#ifndef FONT_DATA_H
#define FONT_DATA_H

#include <stdint.h>

typedef struct {
  uint16_t data_offset;
  uint8_t width;
  uint8_t height;
  uint8_t x_ad;
  uint8_t x_off;
  uint8_t y_off;
} Glyph;

#define GLYPH_WIDTH         8
#define GLYPH_HEIGHT        8

extern uint8_t font_data[128][8];

#endif
