#include <stdio.h>
#include "drawing.h"

void clear() {
    printf("%s", "\033[H");
}

void putpix(int buffer[], int x, int y, color_t color) {
    float a = (color >> 24 & 0xFF) / 255.0f;
    int r2 = color >> 16 & 0xFF;
    int g2 = color >> 8 & 0xFF;
    int b2 = color & 0xFF;
    
    int r1 = buffer[SIDX(x, y)] >> 16 & 0xFF;
    int g1 = buffer[SIDX(x, y)] >> 8 & 0xFF;
    int b1 = buffer[SIDX(x, y)] & 0xFF;

    color_t final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

    buffer[SIDX(x, y)] = final;
}

void blitcol(int buffer[], int minx, int miny, int maxx, int maxy, color_t color) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            putpix(buffer, x, y, color);
        }
    }
}

void blitimg(int buffer[], int x, int y, image img, int mode) {
    int a = x-mode*(img->width/2);
    int b = y-mode*(img->height/2);
    int w = img->width;
    for (int j = 0; j < img->width; j++) {
        for (int k = 0; k < img->height; k++) {
            putpix(buffer, a+j, b+k, img->data[IDX(j, k, w)]);
        }
    }
}

void blitsprite(int buffer[], int x, int y, int ix, int iy, int iw, int ih, image img, int mode) {
    int a = x-mode*(iw/2);
    int b = y-mode*(ih/2);
    for (int j = 0; j < iw; j++) {
        for (int k = 0; k < ih; k++) {
            putpix(buffer, a+j, b+k, img->data[IDX(j+ix, k+iy, iw)]);
        }
    }
}

void blittile(int buffer[], int x, int y, tilemap map, int idx, int mode) {
    int a = x-mode*(map->tw/2);
    int b = y-mode*(map->th/2);
    int tx = map->tiles[idx].x;
    int ty = map->tiles[idx].y;
    int w = map->map->width;
    for (int j = 0; j < map->tw; j++) {
        for (int k = 0; k < map->th; k++) {
            putpix(buffer, a+j, b+k, map->map->data[IDX(tx+j, ty+k, w)]);
        }
    }
}

void blitlayout(int buffer[], int x, int y, tilemap map, tilemapdata dat) {
    for (int j = 0; j < dat->width; j++) {
        for (int k = 0; k < dat->height; k++) {
            blittile(buffer, x+dat->data[IDX(j, k, dat->width)]*16, y+dat->data[IDX(j, k, dat->width)]*16, map, IDX(j, k, dat->width), TOP_LEFT);
        }
    }
}