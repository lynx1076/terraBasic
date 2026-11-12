#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include "font_data.h"
#include "str.h"
#include <raylib.h>
#include <stdint.h>

#define TERMINAL_WIDTH_C      106
#define TERMINAL_HEIGHT_C     60

#define TERMINAL_SCALE        2

#define TERMINAL_WIDTH        TERMINAL_WIDTH_C * GLYPH_WIDTH
#define TERMINAL_HEIGHT       TERMINAL_HEIGHT_C * GLYPH_HEIGHT

typedef struct {
  uint8_t frame_buff[TERMINAL_HEIGHT][TERMINAL_WIDTH];
  Str input_string;
  bool interrupt;
  Color background_color;
  Color text_color;

  unsigned int cx;
  unsigned int cy;
} Terminal;


int term_init(Terminal* term, const char* window_title);
int term_update(Terminal* term);
int term_set_pixel(Terminal* term, unsigned int x, unsigned int y, bool pixel_on);
int term_draw_glyph(Terminal* term, char c, unsigned int x, unsigned int y);
int term_print_glyph(Terminal* term, char c);
bool term_get_interrupt(Terminal* term);

#endif
