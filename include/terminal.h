#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include "font_data.h"
#include "str.h"
#include <raylib.h>
#include <stdint.h>

#define TERMINAL_WIDTH_C          106
#define TERMINAL_HEIGHT_C         60

#define TERMINAL_SCALE            2

#define TERMINAL_WIDTH            TERMINAL_WIDTH_C * GLYPH_WIDTH
#define TERMINAL_HEIGHT           TERMINAL_HEIGHT_C * GLYPH_HEIGHT

typedef struct {
  Color frame_buff[TERMINAL_HEIGHT][TERMINAL_WIDTH];
  Texture2D tex;
  Str input_string;
  bool input_await;
  bool input_ready;
  bool interrupt;
  Color background_color;
  Color text_color;

  unsigned int cx;
  unsigned int cy;
} Terminal;


int term_init(Terminal* term, const char* window_title);
void term_free(Terminal* term);
int term_update(Terminal* term);
int term_get_input(Terminal* term, Str* str);
int term_set_pixel(Terminal* term, unsigned int x, unsigned int y, bool pixel_on);
int term_draw_glyph(Terminal* term, char c, unsigned int x, unsigned int y);
int term_print_glyph(Terminal* term, char c);
int term_printf(Terminal* term, const char* fmt, ...);
int term_scroll(Terminal* term, unsigned int lines);
bool term_get_interrupt(Terminal* term);

#endif
