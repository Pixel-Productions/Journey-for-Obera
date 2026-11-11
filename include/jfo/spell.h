#include <math.h>
#include "../falltree/engine.h"
#include "assets.h"
#include "constants.h"

#ifndef JFO_SPELL_H
#define JFO_SPELL_H

enum SPELL_USE_TYPE {
    EV = 0,
    BO = 1,
    PO = 2
};

enum SPELL_LOCATION_TYPE {
    PEACE = 0,
    BATTLE = 1
};

typedef void (*spell_use_func)(game, enum SPELL_LOCATION_TYPE);

typedef struct {
    unsigned char id;

    char *name;

    enum SPELL_USE_TYPE type;

    unsigned char pd;
    spell_use_func use;
} spell;


static vec2 get_facevec(int mul) {
    switch (animated_sprite_get_frame(shoura) / 4) {
        case 0: return (vec2) { .x = 0, .y = mul };
        case 1: return (vec2) { .x = -mul, .y = 0 };
        case 2: return (vec2) { .x = 0, .y = -mul };
        case 3: return (vec2) { .x = mul, .y = 0 };
    }
}

static void EVfireball1(game gm, enum SPELL_LOCATION_TYPE type) {
    spawn_particle(
        mainscn,
        img_particle_fireball,
        -1,
        shoura->transform.pos,
        get_facevec(2)
    );
}


static member get_nearest_member() {
    float least = -1.0f;
    int mem = 0;

    for (int i = 0; i < memc; i++) {
        float basex = party[1].obj->transform.pos.x - shoura->transform.pos.x;
        float basey = party[1].obj->transform.pos.y - shoura->transform.pos.y;

        float dist = sqrt(basex*basex + basey*basey);

        if (dist < least || least == -1.0f) {
            least = dist;
            mem = i;
        }
    }

    return party[mem];
}

static void EVheal1(game gm, enum SPELL_LOCATION_TYPE type) {
    member mem = get_nearest_member();

    spawn_particle(
        mainscn,
        img_particle_heal,
        12,
        mem.obj->transform.pos,
        (vec2) { .x = 0, .y = 1 }
    );

    mem.hp = MIN(mem.hp + 5, 20);
}

spell spellreg[] = {
    {
        .id = 0,
        .name = "EV Fireball I",

        .type = EV,
        .pd = 5,
        .use = EVfireball1
    },
    {
        .id = 1,
        .name = "EV Heal I",

        .type = EV,
        .pd = 4,
        .use = EVheal1
    }
};

#endif