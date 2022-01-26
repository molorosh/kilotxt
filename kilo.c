
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode(){
    // re-applies the user's previous terminal settings
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(){
    // saves a copy of the user's current terminal settings
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    // create a new copy of the current settings
    struct termios raw = orig_termios;
    // turns off automatic echoing of keystrokes to STDIN
    tcgetattr(STDIN_FILENO, &raw);
    // disable ECHO : stops typing appearing in terminal
    // disable ICANON : change from by-line to by-key
    raw.c_lflag &= ~(ECHO|ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();
    char c;
    while (
        read(STDIN_FILENO, &c, 1) == 1
        &&
        c != 'q'
        );
    return 0;
}
