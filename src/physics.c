#include "physics.h"

void collrect(bool buffer[], int minx, int miny, int maxx, int maxy) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            if (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT) buffer[SIDX(x, y)] = true;
        }
    }
}

void moveandcoll(bool buffer[], vec2 vel, vec2 *pos) {
    if (buffer[SIDX(SCREEN_WIDTH/2+9, SCREEN_HEIGHT/2)] && vel.x > 0 || buffer[SIDX(SCREEN_WIDTH/2-9, SCREEN_HEIGHT/2)] && vel.x < 0) vel.x = 0;
    if (buffer[SIDX(SCREEN_WIDTH/2, SCREEN_HEIGHT/2+9)] && vel.y < 0 || buffer[SIDX(SCREEN_WIDTH/2, SCREEN_HEIGHT/2-9)] && vel.y > 0) vel.y = 0;
    pos->x += vel.x;
    pos->y += vel.y;
}