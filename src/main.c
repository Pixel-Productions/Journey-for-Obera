#define MA_IMPLEMENTATION

#include "engine.h"
#include "images.h"
#include "drawing.h"
#include "physics.h"
#include "miniaudio.h"

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
        free_image(itemregistry[i].texture);
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
        free_image(spellregistry[i].texture);
    }
}

game oneday;

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

int main() {
    oneday = init_game(SCREEN_WIDTH, SCREEN_HEIGHT);

    prect = rect2_init(SCREEN_WIDTH/2-8, SCREEN_HEIGHT/2-8, 16, 16);

    font_default = load_image("assets/textures/fonts/font_default.bmp");

    shoura_down = load_image("assets/textures/shoura_down.bmp");
    shoura_up = load_image("assets/textures/shoura_up.bmp");
    shoura_left = load_image("assets/textures/shoura_left.bmp");
    shoura_right = load_image("assets/textures/shoura_right.bmp");

    overworld = load_tileset("assets/textures/tilemaps/overworld.bmp", 16, 16, 0, 0, 0, 0, USER_DEFINED);
    tileset_add_tile(overworld, 0, 0, true);
    tileset_add_tile(overworld, 1, 0, false);
    tileset_add_tile(overworld, 2, 0, false);
    tileset_add_tile(overworld, 3, 0, false);
    tileset_add_tile(overworld, 1, 1, false);
    tileset_add_tile(overworld, 2, 1, false);
    tileset_add_tile(overworld, 3, 1, false);
    tileset_add_tile(overworld, 1, 2, false);
    tileset_add_tile(overworld, 2, 2, false);
    tileset_add_tile(overworld, 3, 2, false);

    overworld_dat = load_tilemap("assets/data/tilemaps/overworld.bin");

    registeritem(0, "Apple", load_image("assets/textures/items/apple.bmp"));

    invadditem(getitem(0));
    invadditem(getitem(0));

    registerspell(0, "Fireball", DAMAGE, PROJECTILE, 5, load_image("assets/textures/spells/fireball.bmp"));
    registerspell(1, "Fireball2", DAMAGE, PROJECTILE, 5, load_image("assets/textures/spells/fireball.bmp"));

    // Ready game
    pos.x = 0;
    pos.y = 0;

    vel.x = 0;
    vel.y = 0;

    while (!quit) {
        game_tick(oneday);

        if (oneday->input['w']) {
            vel.y = MAX(vel.y - 1, -maxspeed);
            facingdir = 0;
        } else if (oneday->input['s']) {
            vel.y = MIN(vel.y+1, maxspeed);
            facingdir = 2;
        } else if (vel.y != 0)
            vel.y -= (vel.y / abs(vel.y));
        
        if (oneday->input['d']) {
            vel.x = MIN(vel.x+1, maxspeed);
            facingdir = 1;
        } else if (oneday->input['a']) {
            vel.x = MAX(vel.x-1, -maxspeed);
            facingdir = 3;
        } else if (vel.x != 0)
            vel.x -= (vel.x > 0 ? 1 : -1);
        
        if (oneday->input['e'])
            iopen = !iopen;
        
        if (oneday->input['`'])
            quit = 1;

        blit_color(oneday, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR(50, 127, 255, 255));
        blit_tilemap(oneday, -pos.x, -pos.y, overworld, overworld_dat, TOP_LEFT);

        blit_image(oneday, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, (facingdir == 0 ? shoura_up : facingdir == 1 ? shoura_right : facingdir == 2 ? shoura_down : shoura_left), CENTERED);

        if (iopen) {
            blit_color(oneday, 10, 10, SCREEN_WIDTH-10, SCREEN_HEIGHT-10, COLOR(0, 0, 0, 180));
            blit_string(oneday, SCREEN_WIDTH/2, 15, font_default, "Menu", 4, CENTERED);

            for (int i = 0; i < sc; i++) {
                int x = (SCREEN_WIDTH/2-18)+(i%2)*9;
                int y = 24+(int)(i/4)*9;
                blit_image(oneday, x, y, spellregistry[i].texture, CENTERED);
            }

            for (int i = 0; i < invc; i++) {
                int x = SCREEN_WIDTH/2+(i%4)*9;
                int y = 24+(int)(i/4)*9;
                blit_image(oneday, x, y, inventory[i].itm.texture, CENTERED);
            }
        }

        move_and_collide(oneday, vel, &pos, prect);

        time += 1;
    }

    freeitems();
    freespells();

    free_image(font_default);
    free_image(shoura_down);
    free_image(shoura_up);
    free_image(shoura_left);
    free_image(shoura_right);
    
    free_tileset(overworld);
    free_tilemap(overworld_dat);

    end_game(oneday);

    return 0;
}