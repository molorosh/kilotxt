/*** includes ***/

#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/*** defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)

/*** data ***/

struct termios orig_termios;

/*** terminal ***/

void die(const char *s){
    perror(s);
    exit(1);
}

void disableRawMode(){
    // re-applies the user's previous terminal settings
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1){
        die("tssetattr");
    }
}

void enableRawMode(){
    // saves a copy of the user's current terminal settings
    if(tcgetattr(STDIN_FILENO, &orig_termios) == -1){
        die("tsgetattr");
    }
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
    // set some timeouts for waiting for input
    // that means that waiting for input won't halt forever
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    // set the new raw mode settings
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1){
        die("tcsetattr (final)");
    }
}

/*** init ***/

int main() {
    enableRawMode();
    
    while (1){
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        if(iscntrl(c)){
            printf("%d\r\n", c);
        }else{
            printf("%d ('%c')\r\n", c, c);
        }
        if(c == CTRL_KEY('q')) break;
    }

    return 0;
}
