#include <stdlib.h>
#include <stdbool.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

shape shape_init(int w, int h) {
    shape shp = (shape)malloc(sizeof(*shp));
    shp->width = w;
    shp->height = h;

    shp->data = (bool*)malloc(w*h);

    return shp;
}

void blend_shapes(shape sh1, shape sh2) {
    for (int i = 0; i < sh1->width*sh1->height; i++) {
        sh1->data[i] = sh1->data[i] || sh2->data[i];
    }
}

void move_and_collide(game gm, object obj) {
    if (obj->can_collide) {
        for (int x = 0; x < obj->shp->width; x++) {
            for (int y = 0; y < obj->shp->height; y++) {
                if (
                    !obj->shp->data[IDX(x+1, y, obj->shp->width)] && !gm->cbuf[IDX(obj->pos.x+x+obj->vel.x, obj->pos.y+y, gm->width)] && obj->vel.x > 0
                    || !obj->shp->data[IDX(x-1, y, obj->shp->width)] && !gm->cbuf[IDX(obj->pos.x+x-obj->vel.x, obj->pos.y+y, gm->width)] && obj->vel.x < 0
                ) obj->vel.x = 0;
                if (
                    !obj->shp->data[IDX(x, y+1, obj->shp->width)] && !gm->cbuf[IDX(obj->pos.x+x, obj->pos.y+y+obj->vel.y, gm->width)] && obj->vel.y > 0
                    || !obj->shp->data[IDX(x, y-1, obj->shp->width)] && !gm->cbuf[IDX(obj->pos.x+x, obj->pos.y+y-obj->vel.y, gm->width)] && obj->vel.y < 0
                ) obj->vel.y = 0;
            }
        }
    }

    obj->pos.x += obj->vel.x;
    obj->pos.y += obj->vel.y;
}

#ifdef __cplusplus
}
#endif