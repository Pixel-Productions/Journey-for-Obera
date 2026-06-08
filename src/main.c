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
#include "items.h"
#include "physics.h"
#include "ui.h"
#include "buffering.h"

#pragma region SCREEN BUFFERS

#define BUF_LEN SCREEN_WIDTH*SCREEN_HEIGHT
#define BUF_SIZE BUF_LEN*sizeof(color_t)

buffer fbuf;
buffer bbuf;
bool cbuf[BUF_LEN] = { false };

#pragma endregion

#pragma region ASSETS

image font_default;

image shoura_down;
image shoura_up;
image shoura_left;
image shoura_right;

tileset overworld;
tilemap overworld_dat;

item apple;

screen inventory_scr;

#pragma endregion

#pragma region PLAYER DATA

int time = 0;

vec2 pos;
vec2 vel;
int maxspeed = 2;
int facingdir = 0;

bool iopen = false;
item inventory[36];

#pragma endregion

bool inputmap[255] = { false };

int main() {
    fbuf = createbuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    bbuf = createbuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    printf("\e[?25l");

    font_default = loadimage("assets/textures/fonts/font_default.bmp");

    shoura_down = loadimage("assets/textures/shoura_down.bmp");
    shoura_up = loadimage("assets/textures/shoura_up.bmp");
    shoura_left = loadimage("assets/textures/shoura_left.bmp");
    shoura_right = loadimage("assets/textures/shoura_right.bmp");

    overworld = loadtileset("assets/textures/tilemaps/overworld.bmp", 16, 16, 0, 0, 0, 0, USER_DEFINED);
    tilesetadd(overworld, 0, 0, true);
    tilesetadd(overworld, 1, 0, false);
    tilesetadd(overworld, 2, 0, false);
    tilesetadd(overworld, 3, 0, false);
    tilesetadd(overworld, 1, 1, false);
    tilesetadd(overworld, 2, 1, false);
    tilesetadd(overworld, 3, 1, false);
    tilesetadd(overworld, 1, 2, false);
    tilesetadd(overworld, 2, 2, false);
    tilesetadd(overworld, 3, 2, false);

    overworld_dat = loadtilemap("assets/data/tilemaps/overworld.bin");

    apple = loaditem("assets/data/items/apple.bin", "assets/textures/items/apple.bmp");

    inventory_scr = loadscreen("assets/data/ui/inventory.bin");

    // Ready game
    pos.x = 0;
    pos.y = 0;

    vel.x = 0;
    vel.y = 0;

    // Input setup
    scanon();

    while (!inputmap['`']) {
        getinput(inputmap);

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
        
        if (inputmap['e'])
            iopen = !iopen;
        
        memset(cbuf, false, sizeof(cbuf));

        blitcol(fbuf, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR(50, 127, 255, 255));
        blittilemap(fbuf, cbuf, -pos.x, -pos.y, overworld, overworld_dat, TOP_LEFT);
        blitimg(fbuf, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, (facingdir == 0 ? shoura_up : facingdir == 1 ? shoura_right : facingdir == 2 ? shoura_down : shoura_left), CENTERED);
        
        if (iopen) {
            blitscreen(fbuf, inventory_scr);
        }

        moveandcoll(cbuf, &vel, &pos);

        if (memcmp(fbuf, bbuf, BUF_SIZE) != 0) {
            memcpy(bbuf, fbuf, BUF_SIZE);

            clear();
            render(fbuf);
        }

        time += 1;

        usleep(33000);
    }

    freeimage(font_default);
    freeimage(shoura_down);
    freeimage(shoura_up);
    freeimage(shoura_left);
    freeimage(shoura_right);
    
    freetileset(overworld);
    freetilemap(overworld_dat);

    scanoff();

    printf("\e[?25h");

    freebuffer(fbuf);
    freebuffer(bbuf);

    return 0;
}