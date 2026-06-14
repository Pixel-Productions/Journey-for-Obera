#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <memory.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define COLOR(r, g, b, a) ((color_t)((a) << 24 | (r) << 16 | (g) << 8 | (b)))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) ((a)+((b)-(a))*(x))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))


typedef unsigned int color_t;

typedef struct {
    int width, height;
    int bsize;

    color_t *fbuf;
    color_t *bbuf;
    bool *cbuf;

    bool input[255];
} *game;

static struct termios old;

game init_game(int width, int height) {
    // Start terminal window(only supports windows & linux currently)
    #ifdef __WIN32
        system("start cmd");
    #else
        //system("idfk dude, ive tried everything");
    #endif

    // Set terminal width and height and hide text caret
    printf("\e[8;%d;%d", height/2, width);
    printf("\e[?25l");

    // Set up input
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

    // Initialize game struct
    game gm = (game)malloc(sizeof(*gm));

    gm->width = width;
    gm->height = height;

    gm->bsize = width*height;

    gm->fbuf = (color_t*)malloc(width*height*sizeof(color_t));
    gm->bbuf = (color_t*)malloc(width*height*sizeof(color_t));
    gm->cbuf = (bool*)malloc(width*height);

    return gm;
}

void game_tick(game gm) {
    // Parse keyboard input
    char c;

    memset(gm->input, false, 255);

    while (read(STDIN_FILENO, &c, 1) != 0) {
        gm->input[c] = true;
    }

    // Clear collision buffer
    memset(gm->cbuf, false, gm->bsize);
    
    // Swap buffers
    if (memcmp(gm->bbuf, gm->fbuf, gm->bsize*sizeof(color_t)) != 0) {
        memcpy(gm->fbuf, gm->bbuf, gm->bsize*sizeof(color_t));
        
        printf("\033[H");
        for (int y = 0; y < gm->height; y += 2) {
            for (int x = 0; x < gm->width; x++) {
                printf("\033[48;2;%d;%d;%d;38;2;%d;%d;%dm▄", gm->fbuf[IDX(x, y, gm->width)] >> 16 & 0xFF, gm->fbuf[IDX(x, y, gm->width)] >> 8 & 0xFF, gm->fbuf[IDX(x, y, gm->width)] & 0xFF, gm->fbuf[IDX(x, y+1, gm->width)] >> 16 & 0xFF, gm->fbuf[IDX(x, y+1, gm->width)] >> 8 & 0xFF, gm->fbuf[IDX(x, y+1, gm->width)] & 0xFF);
            }
            printf("%s", "\033[0m\r\n");
        }
    }

    // Add ~30fps delay
    usleep(33333);
}

void end_game(game gm) {
    // End terminal
    #ifdef __WIN32
        system("taskkill /F /IM cmd.exe");
    #else
        system("killall -9 bash");
    #endif

    // Free up game data
    free(gm->fbuf);
    free(gm->bbuf);
    free(gm->cbuf);

    free(gm);
}

#pragma endregion

#pragma region UTILS

typedef struct {
    int x, y;
} vec2;

typedef struct {
    int x, y;
    int width, height;
} rect2;

vec2 vec2_init(int x, int y) {
    vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

rect2 rect2_init(int x, int y, int width, int height) {
    rect2 r;
    
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    return r;
}

#ifdef __cplusplus
}
#endif