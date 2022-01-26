
#include <ctype.h>
#include <stdio.h>
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
    // input flags:
    // IXON : disable terminal pausing
    // ICRNL : disable input correction of newlines
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    // output flags:
    // OPOST : disable output correction of newlines
    raw.c_oflag &= ~(OPOST);
    // CS8 : set character size to 8 bits per byte
    raw.c_cflag |= (CS8);
    // local flags:
    // ECHO : stops typing appearing in terminal
    // ICANON : change from by-line to by-key
    // ISIG : prevents app from raising SIGINT events
    // IEXTEN : disable Ctrl-V
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();
    char c;
    while (
        read(STDIN_FILENO, &c, 1) == 1
        &&
        c != 'q'
        ){
            if(iscntrl(c)){
                printf("%d\r\n", c);
            }else{
                printf("%d ('%c')\r\n", c, c);
            }
        }
    return 0;
}
