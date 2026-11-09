#include "terminal.h"
#include "font_data.h"
#include "result.h"
#include <raylib.h>
#include <stddef.h>


char text_buff[TERMINAL_HEIGHT][TERMINAL_WIDTH];
int cursor_x, cursor_y = 0;


int term_init(const char* window_title) {
  SetTraceLogLevel(LOG_ERROR);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(GLYPH_WIDTH * TERMINAL_WIDTH, GLYPH_HEIGHT * TERMINAL_HEIGHT, window_title);
  SetExitKey(KEY_NULL);

  if (!IsWindowReady()) {
    RES_RETURN(r_EUNKNOWN, -1);
  }

  for (size_t y = 0; y < TERMINAL_HEIGHT; y++) {
    for (size_t x = 0; x < TERMINAL_WIDTH; x++) {
      text_buff[y][x] = ' ';
    }
  }

  RES_RETURN(r_ENONE, 0);
}

int term_update(void) {
  BeginDrawing();

  for (size_t y = 0; y < TERMINAL_HEIGHT; y++) {
    for (size_t x = 0; x < TERMINAL_WIDTH; x++) {
      text_buff[y][x] = ' ';
    }
  }

  EndDrawing();

  RES_RETURN(r_ENONE, 0);
}

int term_draw_glyph(char c, unsigned int y, unsigned int x) {
  for (size_t bmy = 0; bmy < GLYPH_HEIGHT; bmy++) {
    for (size_t bmx = 0; bmx < GLYPH_WIDTH; bmx++) {
}

