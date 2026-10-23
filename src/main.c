#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include "interpreter.h"
#include "result.h"
#include "str.h"
#include "vector.h"
#include "tokens.h"
#include "lexer.h"

int window_width = 800;
int window_height = 600;

char* window_title = "terraTree";

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  Vec tokens;
  if (vec_init(&tokens, sizeof(Token))) return _res;

  Str error;
  Str line;

  if (str_init_cstr(&error, "Unknown error")) return _res;

  if (str_init_cstr(&line, "goto 20 var lEt x = 0 20.1a*b")) return _res;
  str_cstr(&line);
  printf("Processing string (%zu): %s\n", line.length, line.text);

  Itp itp;
  if (itp_init(&itp)) return _res;

  if (itp_line_add(&itp, &line, 1)) return _res;
  if (itp_print_line(&itp, 1)) return _res;

  return 0;

  SetTraceLogLevel(LOG_ERROR);
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(window_width, window_height, window_title);
  SetExitKey(KEY_NULL);

  if (!IsWindowReady()) {
    return 0;
  }

  SetTargetFPS(30);

  while (!WindowShouldClose()) {
    window_width = GetScreenWidth();
    window_height = GetScreenHeight();
    
    BeginDrawing();
    ClearBackground(WHITE);

    EndMode2D();

    EndDrawing();
  }

  return 0;
}

