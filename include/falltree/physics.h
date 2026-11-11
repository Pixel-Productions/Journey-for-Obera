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

    shp->data = (bool*)calloc(w*h, 1);

    return shp;
}

void move_and_collide(game gm, object obj) {
    if (obj->can_collide) {
        shape shp = obj->shp;

        for (int x = 0; x < shp->width; x++) {
            for (int y = 0; y < shp->height; y++) {
                if (shp->data[IDX(x, y, shp->width)]) {
                    int wx = obj->transform.offset.x + obj->transform.pos.x + x;
                    int wy = obj->transform.offset.y + obj->transform.pos.y + y;

                    int tx = wx+obj->vel.x;
                    int ty = wy+obj->vel.y;

                    if (wx >= 0 && wx < gm->width && wy >= 0 && wy < gm->height) {
                        if (gm->cbuf[IDX(tx, wy, gm->width)] != 0 && gm->cbuf[IDX(tx, wy, gm->width)] != obj->uid) obj->vel.x = 0;
                        if (gm->cbuf[IDX(wx, ty, gm->width)] != 0 && gm->cbuf[IDX(wx, ty, gm->width)] != obj->uid) obj->vel.y = 0;
                    }
                }
            }
        }
    }

    obj->transform.pos.x += obj->vel.x;
    obj->transform.pos.y += obj->vel.y;
}

#ifdef __cplusplus
}
#endif