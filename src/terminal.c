#include "terminal.h"
#include "debug.h"
#include "font_data.h"
#include "result.h"
#include "str.h"
#include <stdarg.h>
#include <raylib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

  Image img = {
    .data = term->frame_buff,
    .width = TERMINAL_WIDTH,
    .height = TERMINAL_HEIGHT,
    .mipmaps = 1,
    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
  };

  term->tex = LoadTextureFromImage(img);
  SetTextureFilter(term->tex, TEXTURE_FILTER_POINT);

  if (str_init(&term->input_string)) return -1;

  term->cx = 0;
  term->cy = 0;

  term->input_await = false;
  term->input_ready = false;
  term->interrupt = false;

  RES_RETURN(r_ENONE, 0);
}

void term_free(Terminal* term) {
  ASSERT(term);

  str_free(&term->input_string);
  UnloadTexture(term->tex);
}

int term_update(Terminal* term) {
  BeginDrawing();
  ClearBackground(WHITE);

  if (WindowShouldClose()) term->interrupt = true;

  UpdateTexture(term->tex, term->frame_buff);
  DrawTextureEx(term->tex, (Vector2){0, 0}, 0, TERMINAL_SCALE, WHITE);

  bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

  if (ctrl) {
    if (IsKeyPressed(KEY_C)) {
      term->interrupt = true;
      term->input_await = false;
      term->input_ready = false;
      term_printf(term, "\nCTRL+C\n");
    }
  }

  if (!ctrl && term->input_await) {
    if (IsKeyPressed(KEY_ENTER)) {
      term->input_ready = true;
      term_printf(term, "\n");
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      size_t len = term->input_string.length;

      if (len) {
        len = len ? len - 1 : 0;
        if (term->cx) term->cx--;
        if (term_draw_glyph(term, ' ', term->cx * GLYPH_WIDTH, term->cy * GLYPH_HEIGHT)) return -1;

        if (str_substr(&term->input_string, 0, len)) return -1;
      }
    }

    if (!term->input_ready) {
      char c = GetCharPressed();
      while (c != '\0') {
        if (str_cncat(&term->input_string, &c, 1)) return -1;
        term_printf(term, "%c", c);
        c = GetCharPressed();
      }
    }
  }

  EndDrawing();

  RES_RETURN(r_ENONE, 0);
}

int term_get_input(Terminal* term, Str* str) {
  ASSERT(term);
  ASSERT(str);

  if (term->input_ready) {
    if (str_cpy(str, &term->input_string)) return -1;
    term->input_ready = false;
    term->input_await = false;

    RES_RETURN(r_DATA_READY, 1);
  }

  if (!term->input_await) {
    if (str_clear(&term->input_string)) return -1;
  }

  term->input_await = true;
  term->input_ready = false;

  RES_RETURN(r_ENONE, 0);
}

int term_set_pixel(Terminal* term, unsigned int x, unsigned int y, bool pixel_on) {
  ASSERT(term);

  if (x >= TERMINAL_WIDTH || y >= TERMINAL_HEIGHT) RES_RETURN(r_EBOUNDS, -1);

  term->frame_buff[y][x] = pixel_on ? term->text_color : term->background_color;

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

  switch (c) {
    case '\n': {
      term->cx = 0;
      term->cy++;
    } break;
    default: {
      if (term_draw_glyph(term, c, term->cx * GLYPH_WIDTH, term->cy * GLYPH_HEIGHT)) return -1;
      term->cx++;
    } break;
  }

  term->cy += term->cx / TERMINAL_WIDTH_C;
  term->cx = term->cx % TERMINAL_WIDTH_C;

  if (term->cy >= TERMINAL_HEIGHT_C) {
    if (term_scroll(term, term->cy - TERMINAL_HEIGHT_C + 1)) return -1;
    term->cy = TERMINAL_HEIGHT_C - 1;
  }

  RES_RETURN(r_ENONE, 0);
}

int term_printf(Terminal* term, const char* fmt, ...) {
  ASSERT(term);
  ASSERT(fmt);

  va_list va;
  va_start(va, fmt);
  char* buff;
  int bytes = vasprintf(&buff, fmt, va);
  va_end(va);

  if (bytes < 0) RES_RETURN(r_EUNKNOWN, -1);

  for (size_t i = 0; i < (size_t)bytes; i++) {
    if (term_print_glyph(term, buff[i])) return -1;
  }
  free(buff);

  RES_RETURN(r_ENONE, 0);
}

int term_scroll(Terminal* term, unsigned int lines) {
  ASSERT(term);

  size_t line_mem_offset = TERMINAL_WIDTH * GLYPH_HEIGHT * lines;

  size_t count = (TERMINAL_HEIGHT * TERMINAL_WIDTH - line_mem_offset) * sizeof(Color);
  size_t offset_src = line_mem_offset * sizeof(Color);
  size_t offset_dst = 0;

  memmove((char*)term->frame_buff + offset_dst, (char*)term->frame_buff + offset_src, count);

  for (size_t y = 0; y < GLYPH_HEIGHT; y++) {
    for (size_t x = 0; x < TERMINAL_WIDTH; x++) {
      if (term_set_pixel(term, x, TERMINAL_HEIGHT - y - 1, 0)) return -1;
    }
  }

  RES_RETURN(r_ENONE, 0);
}

bool term_get_interrupt(Terminal* term) {
  ASSERT(term);

  bool cpy = term->interrupt;
  term->interrupt = false;
  return cpy;
}

