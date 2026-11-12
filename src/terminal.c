#include "terminal.h"
#include "debug.h"
#include "font_data.h"
#include "result.h"
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>


int term_init(Terminal* term, const char* window_title) {
  SetTraceLogLevel(LOG_ERROR);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(TERMINAL_WIDTH * TERMINAL_SCALE, TERMINAL_HEIGHT * TERMINAL_SCALE, window_title);
  SetExitKey(KEY_NULL);

  if (!IsWindowReady()) {
    RES_RETURN(r_EUNKNOWN, -1);
  }

  for (int y = 0; y < TERMINAL_HEIGHT; y++) {
    for (int x = 0; x < TERMINAL_WIDTH; x++) {
      if (term_set_pixel(term, x, y, 0)) return -1;
    }
  }

  term->cx = 0;
  term->cy = 0;

  RES_RETURN(r_ENONE, 0);
}

int term_update(Terminal* term) {
  BeginDrawing();
  ClearBackground(WHITE);

  if (WindowShouldClose()) RES_RETURN(r_INTERRUPTED, -1);

  for (char key = GetCharPressed(); key > 0; key = GetCharPressed()) {
    if (term_print_glyph(term, key)) return -1;
  }

  for (int y = 0; y < TERMINAL_HEIGHT; y++) {
    for (int x = 0; x < TERMINAL_WIDTH; x++) {
      for (int sy = 0; sy < TERMINAL_SCALE; sy++) {
        for (int sx = 0; sx < TERMINAL_SCALE; sx++) {
          DrawPixel(x * TERMINAL_SCALE + sx, y * TERMINAL_SCALE + sy, term->frame_buff[y][x] ? term->text_color : term->background_color);
        }
      }
    }
  }

  EndDrawing();

  RES_RETURN(r_ENONE, 0);
}

int term_set_pixel(Terminal* term, unsigned int x, unsigned int y, bool pixel_on) {
  ASSERT(term);

  if (x >= TERMINAL_WIDTH || y >= TERMINAL_HEIGHT) RES_RETURN(r_EBOUNDS, -1);

  term->frame_buff[y][x] = pixel_on ? -1 : 0;

  RES_RETURN(r_ENONE, 0);
}

int term_draw_glyph(Terminal* term, char c, unsigned int x, unsigned int y) {
  uint8_t* bytes = font_data[(int)c];

  for (int bmy = 0; bmy < GLYPH_HEIGHT; bmy++) {
    for (int bmx = 0; bmx < GLYPH_WIDTH; bmx++) {
      bool pixel = bytes[bmy] & (1 << bmx);

      if (term_set_pixel(term, x + bmx, y + bmy, pixel)) return -1;
    }
  }

  RES_RETURN(r_ENONE, 0);
}

int term_print_glyph(Terminal* term, char c) {
  ASSERT(term);

  if (term_draw_glyph(term, c, term->cx * GLYPH_WIDTH, term->cy * GLYPH_HEIGHT)) return -1;

  term->cx++;
  term->cy += term->cx / TERMINAL_WIDTH_C;
  term->cx = term->cx % TERMINAL_WIDTH_C;

  RES_RETURN(r_ENONE, 0);
}

bool term_get_interrupt(Terminal* term) {
  ASSERT(term);

  bool cpy = term->interrupt;
  term->interrupt = false;
  return cpy;
}

