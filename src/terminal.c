#include <stdatomic.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <poll.h>
#include <termios.h>
#include <stdbool.h>

struct termios orig_termios;
volatile atomic_bool terminal_int = false;

void _sigint_handler(int sig) {
  (void)sig;

  terminal_int = true;
}

bool got_interrupt(void) {

}

void _reset_terminal_mode(void) {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void set_conio_mode(void) {
  struct termios new_termios;
  
  tcgetattr(STDIN_FILENO, &orig_termios);
  atexit(_reset_terminal_mode);
  
  new_termios = orig_termios;
  new_termios.c_lflag &= ~(ICANON | ECHO);
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 0;
  
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

size_t get_input() {
  while (!got_interrupt()) {
    char ch;
        // read() will return immediately because of our termios settings
    int bytes_read = read(STDIN_FILENO, &ch, 1);

    if (bytes_read > 0) {
    // Echo the character manually since ECHO is turned off
    if (ch == '\n' || ch == '\r') {
      printf("\n");
      break; // User pressed Enter, we are done
            } else if (ch == 127 || ch == 8) { // Handle Backspace
                if (count > 0) {
                    count--;
                    buffer[count] = '\0';
                    printf("\b \b"); // Move back, print space to clear, move back
                    fflush(stdout);
                }
            } else if (count < (int)sizeof(buffer) - 1) {
                buffer[count++] = ch;
                putchar(ch);
                fflush(stdout);
            }
        }

        // 4. Sleep briefly to prevent 100% CPU usage loop
        usleep(10000); // 10ms pause
    }
}

