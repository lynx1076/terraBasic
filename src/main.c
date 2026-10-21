#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include "result.h"
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

  const char* error;
  Str line;

  if (str_init_cstr(&line, "1.2, ( 12 3)\"Hallo test string\"")) return _res;
  str_cstr(&line);
  printf("Processing string (%zu): %s\n", line.length, line.text);

  if (lxr_LexLine(&tokens, &error, &line)) {
    printf("%s\n", error);
  } else {
    for (size_t i = 0; i < tokens.count; i++) {
      Token* token = vec_get(&tokens, i);
      printf("(%zu): ", i);
      tok_print(token);
      printf("\n");
    }
  }


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

