#include <stdio.h>
#include "engine.h"
#include "images.h"
#include "physics.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void put_pixel(game gm, int x, int y, color_t color) {
    if (x >= 0 && x < gm->width && y >= 0 && y < gm->height) {
        float a = (color >> 24 & 0xFF) / 255.0f;
        int r2 = color >> 16 & 0xFF;
        int g2 = color >> 8 & 0xFF;
        int b2 = color & 0xFF;
        
        int r1 = gm->fbuf[IDX(x, y, gm->width)] >> 16 & 0xFF;
        int g1 = gm->fbuf[IDX(x, y, gm->width)] >> 8 & 0xFF;
        int b1 = gm->fbuf[IDX(x, y, gm->width)] & 0xFF;

        int final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

        gm->bbuf[IDX(x, y, gm->width)] = final;
    }
}

void blit_color(game gm, int minx, int miny, int maxx, int maxy, color_t color) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            put_pixel(gm, x, y, color);
        }
    }
}

void blit_image(game gm, int x, int y, image img, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(img.width/2);
    int b = y-mode*(img.height/2);
    int w = img.width;
    for (int j = 0; j < img.width; j++) {
        for (int k = 0; k < img.height; k++) {
            put_pixel(gm, a+j, b+k, img.data[IDX(j, k, w)]);
        }
    }
}

void blit_sprite(game gm, int x, int y, int ix, int iy, int iw, int ih, image img, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(iw/2);
    int b = y-mode*(ih/2);
    for (int j = 0; j < iw; j++) {
        for (int k = 0; k < ih; k++) {
            put_pixel(gm, a+j, b+k, img.data[IDX(j+ix, k+iy, img.width)]);
        }
    }
}

void blit_tile(game gm, int x, int y, tileset set, int idx, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(set->tw/2);
    int b = y-mode*(set->th/2);
    int tx = set->tiles[idx].x;
    int ty = set->tiles[idx].y;
    int w = set->map.width;
    for (int j = 0; j < set->tw; j++) {
        for (int k = 0; k < set->th; k++) {
            put_pixel(gm, a+j, b+k, set->map.data[IDX(tx+j, ty+k, w)]);
        }
    }
    if (set->tiles[idx].hascoll)
        add_collision_rect(gm, x, y, x+set->tw, y+set->th);
}

void blit_tilemap(game gm, int x, int y, tileset set, tilemap map, enum DRAW_IMAGE_MODE mode) {
    int w = map.width;
    for (int j = 0; j < w; j++) {
        for (int k = 0; k < map.height; k++) {
            blit_tile(gm, x+j*16, y+k*16, set, map.data[IDX(j, k, w)], mode);
        }
    }
}

void blit_string(game gm, int x, int y, image font, char str[], size_t len, enum DRAW_IMAGE_MODE mode) {
    for (int i = 0; i < len; i++) {
        blit_sprite(gm, x+i*6-mode*6*len/2, y-mode*3, (str[i]-32)*6, 0, 6, 6, font, TOP_LEFT);
    }
}

#ifdef __cplusplus
}
#endif