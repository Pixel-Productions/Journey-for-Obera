#include <termios.h>
#include <unistd.h>
#include "input.h"

struct termios old;

void scanon() {
    struct termios new;

    tcgetattr(STDIN_FILENO, &old);
    new = old;

    new.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    new.c_oflag &= ~OPOST;
    new.c_cflag &= ~(CSIZE | PARENB);
    new.c_cflag |= CS8;
    new.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);

    new.c_cc[VMIN] = 0;
    new.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void scanoff() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}