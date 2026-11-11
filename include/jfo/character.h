#include "../falltree/engine.h"
#include "constants.h"

#pragma once

void move_character(object obj) {
    if (obj->vel.y > 0) {
        animated_sprite_set_frame(obj, (t / 16) % 4);
    } else if (obj->vel.y < 0) {
        animated_sprite_set_frame(obj, 8 + (t / 16) % 4);
    } else if (obj->vel.x > 0) {
        animated_sprite_set_frame(obj, 12 + (t / 16) % 4);
    } else if (obj->vel.x < 0) {
        animated_sprite_set_frame(obj, 4 + (t / 16) % 4);
    } else {
        animated_sprite_set_frame(obj, ROUND_TO(animated_sprite_get_frame(obj), 4));
    }

    move_and_collide(jfo, obj);
}

void move_steps(object obj, int steps) {
    for (int i = 0; i < steps; i++) {
        t++;
        move_character(obj);
        usleep(16667);
    }
}