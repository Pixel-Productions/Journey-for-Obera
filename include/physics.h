#include <stdbool.h>
#include "engine.h"

#pragma once

void collrect(buffer buf, int minx, int miny, int maxx, int maxy) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            if (x >= 0 && x < buf->width && y >= 0 && y < buf->height)
                buf->data[IDX(x, y, buf->width)] = true;
            //printf("%d", buf->data[IDX(x, y, buf->width)]);
        }
    }
}

void moveandcoll(buffer buf, vec2 vel, vec2 *pos, rect2 rect) {
    if (buf->data[IDX(rect.x+rect.width, rect.y, buf->width)] && vel.x > 0 || buf->data[IDX(rect.x, rect.y, buf->width)] && vel.x < 0) { vel.x = 0; printf("hi"); }
    if (buf->data[IDX(rect.x, rect.y+rect.height, buf->width)] && vel.y > 0 || buf->data[IDX(rect.x, rect.y, buf->width)] && vel.y < 0) vel.y = 0;
    pos->x += vel.x;
    pos->y += vel.y;
}