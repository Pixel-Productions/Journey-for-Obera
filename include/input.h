#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <memory.h>

#pragma once

struct termios old;

void scanon() {
    struct termios nw;

    tcgetattr(STDIN_FILENO, &old);
    nw = old;

    nw.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    nw.c_oflag &= ~OPOST;
    nw.c_cflag &= ~(CSIZE | PARENB);
    nw.c_cflag |= CS8;
    nw.c_lflag &= ~(ICANON | ECHO | ECHONL | ISIG | IEXTEN);

    nw.c_cc[VMIN] = 0;
    nw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &nw);
}

void scanoff() {
    tcsetattr(STDIN_FILENO, TCSANOW, &old);
}

void readinput(bool map[]) {
    char c;

    memset(map, false, 255);

    while (read(STDIN_FILENO, &c, 1) != 0) {
        map[c] = true;
    }
}