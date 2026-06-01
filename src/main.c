#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include <math.h>
#include "engine.h"
#include "images.h"
#include "drawing.h"
#include "input.h"

void render(int buffer[]) {
    for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            printf("\033[48;2;%d;%d;%d;38;2;%d;%d;%dm▄\033[0m", buffer[SIDX(x, y)] >> 16 & 0xFF, buffer[SIDX(x, y)] >> 8 & 0xFF, buffer[SIDX(x, y)] & 0xFF, buffer[SIDX(x, y+1)] >> 16 & 0xFF, buffer[SIDX(x, y+1)] >> 8 & 0xFF, buffer[SIDX(x, y+1)] & 0xFF);
        }
        printf("%s", "\r\n");
    }
}

// Screen buffers
#define BUF_LEN SCREEN_WIDTH*SCREEN_HEIGHT
#define BUF_SIZE BUF_LEN*sizeof(color_t)

color_t fbuf[BUF_LEN] = { 0 };
color_t bbuf[BUF_LEN] = { 0 };

// Images & game data
struct image tree;

struct image shoura_down;
struct image shoura_up;
struct image shoura_left;
struct image shoura_right;

int time = 0;

struct vec2 pos;
struct vec2 vel;
int maxspeed = 2;
int facingdir = 0;

bool inputmap[255] = { false };

int main() {
    // Image loading
    tree = loadimage("assets/textures/fall_tree.bmp");

    shoura_down = loadimage("assets/textures/shoura_down.bmp");
    shoura_up = loadimage("assets/textures/shoura_up.bmp");
    shoura_left = loadimage("assets/textures/shoura_left.bmp");
    shoura_right = loadimage("assets/textures/shoura_right.bmp");

    // Ready game
    pos.x = 0;
    pos.y = 0;

    vel.x = 0;
    vel.y = 0;

    // Input setup
    scanon();

    char c;

    while (!inputmap['`']) {
        memset(inputmap, false, sizeof(inputmap));

        while (read(STDIN_FILENO, &c, 1) != 0) {
            inputmap[c] = true;
        }

        if (inputmap['w']) {
            vel.y = MAX(vel.y - 1, -maxspeed);
            facingdir = 0;
        } else if (inputmap['s']) {
            vel.y = MIN(vel.y+1, maxspeed);
            facingdir = 2;
        } else if (vel.y != 0)
            vel.y -= (vel.y / abs(vel.y));
        
        if (inputmap['d']) {
            vel.x = MIN(vel.x+1, maxspeed);
            facingdir = 1;
        } else if (inputmap['a']) {
            vel.x = MAX(vel.x-1, -maxspeed);
            facingdir = 3;
        } else if (vel.x != 0)
            vel.x -= (vel.x / abs(vel.x));

        pos.x += vel.x;
        pos.y += vel.y;

        memset(fbuf, 0, BUF_SIZE);

        blitcol(fbuf, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR(50, 127, 255, 255));

        blitimg(fbuf, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, (facingdir == 0 ? shoura_up : facingdir == 1 ? shoura_right : facingdir == 2 ? shoura_down : shoura_left), CENTERED);

        if (memcmp(fbuf, bbuf, BUF_SIZE) != 0) {
            memcpy(bbuf, fbuf, BUF_SIZE);

            clear();
            render(fbuf);
        }

        time += 1;

        usleep(33000);
    }

    freeimage(tree);

    freeimage(shoura_down);
    freeimage(shoura_up);
    freeimage(shoura_left);
    freeimage(shoura_right);

    scanoff();

    return 0;
}