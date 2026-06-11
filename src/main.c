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
#include "physics.h"
#include "buffering.h"

#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64

typedef struct {
    int id;
    char *name;
    image texture;
} item;

typedef struct {
    item itm;
    int count;
} itemstack;

int ic = 0;
item itemregistry[1];

void registeritem(int id, char name[], image texture) {
    item itm;

    itm.id = id;
    itm.name = name;
    itm.texture = texture;

    itemregistry[ic] = itm;
    ic++;
}

itemstack getitem(int id) {
    itemstack itmstack;
    itmstack.itm = itemregistry[id];
    itmstack.count = 1;
}

void freeitems() {
    for (int i = 0; i < LENOF(itemregistry); i++) {
        freeimage(itemregistry[i].texture);
    }
}

enum SPELL_EFFECT_TYPE {
    DAMAGE,
    HEAL
};

enum SPELL_CAST_TYPE {
    PROJECTILE,
    BURST,
    CENTRALIZED
};

typedef struct {
    int id;
    char *name;
    enum SPELL_EFFECT_TYPE effect;
    enum SPELL_CAST_TYPE cast;
    int factor;
    image texture;
} spell;

int sc = 1;
spell spellregistry[8];

void registerspell(int id, char name[], enum SPELL_EFFECT_TYPE effect, enum SPELL_CAST_TYPE cast, int factor, image texture) {
    spell spl;

    spl.id = id;
    spl.name = name;
    spl.effect = effect;
    spl.cast = cast;
    spl.texture = texture;

    spellregistry[sc] = spl;
    sc++;
}

void freespells() {
    for (int i = 0; i < LENOF(spellregistry); i++) {
        freeimage(spellregistry[i].texture);
    }
}

#pragma region SCREEN BUFFERS

#define BUF_LEN SCREEN_WIDTH*SCREEN_HEIGHT
#define BUF_SIZE BUF_LEN*sizeof(int)

buffer fbuf;
buffer bbuf;
buffer cbuf;

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

#pragma endregion

#pragma region WORLD DATA

int time = 0;

bool quit = false;

#pragma endregion

#pragma region PLAYER DATA

vec2 pos;
vec2 vel;

rect2 prect;

int maxspeed = 2;
int facingdir = 0;

bool iopen = false;
int invc = 0;
itemstack inventory[16];

void invadditem(itemstack itm) {
    inventory[invc] = itm;
    invc++;
}

item sword;
item shield;

#pragma endregion

bool inputmap[255] = { false };

int main() {
    printf("\e[?25l");

    fbuf = createbuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    bbuf = createbuffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    cbuf = createbuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    prect = newrect2(SCREEN_WIDTH/2-8, SCREEN_HEIGHT/2-8, 16, 16);

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

    registeritem(0, "Apple", loadimage("assets/textures/items/apple.bmp"));

    invadditem(getitem(0));
    invadditem(getitem(0));

    registerspell(0, "Fireball", DAMAGE, PROJECTILE, 5, loadimage("assets/textures/spells/fireball.bmp"));
    registerspell(1, "Fireball2", DAMAGE, PROJECTILE, 5, loadimage("assets/textures/spells/fireball.bmp"));

    // Ready game
    pos.x = 0;
    pos.y = 0;

    vel.x = 0;
    vel.y = 0;

    // Input setup
    scanon();

    while (!quit) {
        readinput(inputmap);

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
        
        if (inputmap['`'])
            quit = 1;
        
        memset(cbuf->data, 0, sizeof(cbuf));

        blitcol(fbuf, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR(50, 127, 255, 255));
        blittilemap(fbuf, cbuf, -pos.x, -pos.y, overworld, overworld_dat, TOP_LEFT);
        blitimg(fbuf, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, (facingdir == 0 ? shoura_up : facingdir == 1 ? shoura_right : facingdir == 2 ? shoura_down : shoura_left), CENTERED);

        if (iopen) {
            blitcol(fbuf, 10, 10, SCREEN_WIDTH-10, SCREEN_HEIGHT-10, COLOR(0, 0, 0, 180));
            blitstr(fbuf, SCREEN_WIDTH/2, 15, font_default, "Menu", 4, CENTERED);

            for (int i = 0; i < sc; i++) {
                int x = (SCREEN_WIDTH/2-18)+(i%2)*9;
                int y = 24+(int)(i/4)*9;
                blitimg(fbuf, x, y, spellregistry[i].texture, CENTERED);
            }

            for (int i = 0; i < invc; i++) {
                int x = SCREEN_WIDTH/2+(i%4)*9;
                int y = 24+(int)(i/4)*9;
                blitimg(fbuf, x, y, inventory[i].itm.texture, CENTERED);
            }
        }

        moveandcoll(cbuf, vel, &pos, prect);

        if (memcmp(fbuf, bbuf, BUF_SIZE) != 0) {
            memcpy(bbuf, fbuf, BUF_SIZE);

            clear();
            render(fbuf);
        }

        time += 1;

        usleep(33000);
    }

    freeitems();
    freespells();

    freeimage(font_default);
    freeimage(shoura_down);
    freeimage(shoura_up);
    freeimage(shoura_left);
    freeimage(shoura_right);
    
    freetileset(overworld);
    freetilemap(overworld_dat);

    scanoff();

    freebuffer(fbuf);
    freebuffer(bbuf);
    freebuffer(cbuf);

    printf("\e[?25h");
    return 0;
}