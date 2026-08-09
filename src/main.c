#include "../include/falltree/engine.h"
#include "../include/falltree/types.h"
#include "../include/falltree/object.h"
#include <unistd.h>

game jfo;
int t = 0;

int max_speed = 3;

void tick(game gm) {
    t++;
}

void shoura_tick(game gm, object obj) {
    if (gm->keyboard['d'])
        obj->vel.x = CLAMP(obj->vel.x+1, -max_speed, max_speed);
    else if (gm->keyboard['a'])
        obj->vel.x = CLAMP(obj->vel.x-1, -max_speed, max_speed);
    else if (obj->vel.x != 0)
        obj->vel.x -= obj->vel.x > 0 ? 1 : -1;

    if (gm->keyboard['s'])
        obj->vel.y = CLAMP(obj->vel.y+1, -max_speed, max_speed);
    else if (gm->keyboard['w'])
        obj->vel.y = CLAMP(obj->vel.y-1, -max_speed, max_speed);
    else if (obj->vel.y != 0)
        obj->vel.y -= obj->vel.y > 0 ? 1 : -1;

    if (obj->vel.y > 0) {
        animated_sprite_set_frame(obj, (t / 4) % 4);
    } else if (obj->vel.y < 0) {
        animated_sprite_set_frame(obj, 8 + (t / 4) % 4);
    } else if (obj->vel.x > 0) {
        animated_sprite_set_frame(obj, 12 + (t / 4) % 4);
    } else if (obj->vel.x < 0) {
        animated_sprite_set_frame(obj, 4 + (t / 4) % 4);
    } else {
        animated_sprite_set_frame(obj, ROUND_TO(animated_sprite_get_frame(obj), 4));
    }

    move_and_collide(gm, obj);
}

int main() {
    jfo = game_init("Journey for Obera", 280, 160, 4);

    load_asset(jfo, "shoura", "assets/textures/shoura.png", IMAGE);

    object shoura = object_init(ANIMATED_SPRITE, get_asset(jfo, "shoura"), 16, 16, 4, 4, 0, 0, 0, 0);
    shoura->tick = shoura_tick;
    list_add_elem(jfo->objects, shoura);

    jfo->tick = tick;
    game_start(jfo);

    end_game(jfo);
    return 0;
}