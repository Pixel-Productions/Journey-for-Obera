#include <stdbool.h>
#include "engine.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void add_collision_rect(game gm, int minx, int miny, int maxx, int maxy) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            if (x >= 0 && x < gm->width && y >= 0 && y < gm->height)
                gm->cbuf[IDX(x, y, gm->width)] = true;
        }
    }
}

void move_and_collide(game gm, vec2 vel, vec2 *pos, rect2 rect) {
    if (gm->cbuf[IDX(rect.x+rect.width+vel.x, rect.y+rect.height/2, gm->width)] && vel.x > 0 || gm->cbuf[IDX(rect.x+vel.x, rect.y+rect.height/2, gm->width)] && vel.x < 0) vel.x = 0;
    if (gm->cbuf[IDX(rect.x+rect.width/2, rect.y+rect.height+vel.y, gm->width)] && vel.y > 0 || gm->cbuf[IDX(rect.x+rect.width/2, rect.y+vel.y, gm->width)] && vel.y < 0) vel.y = 0;
    pos->x += vel.x;
    pos->y += vel.y;
}

#ifdef __cplusplus
}
#endif