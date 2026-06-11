#include <stdio.h>
#include "buffering.h"
#include "physics.h"

#pragma once

void clear() {
    printf("%s", "\033[H");
}

void putpix(buffer buf, int x, int y, color_t color) {
    if (x >= 0 && x < buf->width && y >= 0 && y < buf->height) {
        float a = (color >> 24 & 0xFF) / 255.0f;
        int r2 = color >> 16 & 0xFF;
        int g2 = color >> 8 & 0xFF;
        int b2 = color & 0xFF;
        
        int r1 = buf->data[IDX(x, y, buf->width)] >> 16 & 0xFF;
        int g1 = buf->data[IDX(x, y, buf->width)] >> 8 & 0xFF;
        int b1 = buf->data[IDX(x, y, buf->width)] & 0xFF;

        int final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

        buf->data[IDX(x, y, buf->width)] = final;
    }
}

void blitcol(buffer buf, int minx, int miny, int maxx, int maxy, color_t color) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            putpix(buf, x, y, color);
        }
    }
}

void blitimg(buffer buf, int x, int y, image img, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(img.width/2);
    int b = y-mode*(img.height/2);
    int w = img.width;
    for (int j = 0; j < img.width; j++) {
        for (int k = 0; k < img.height; k++) {
            putpix(buf, a+j, b+k, img.data[IDX(j, k, w)]);
        }
    }
}

void blitsprite(buffer buf, int x, int y, int ix, int iy, int iw, int ih, image img, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(iw/2);
    int b = y-mode*(ih/2);
    for (int j = 0; j < iw; j++) {
        for (int k = 0; k < ih; k++) {
            putpix(buf, a+j, b+k, img.data[IDX(j+ix, k+iy, img.width)]);
        }
    }
}

void blittile(buffer buf, buffer cbuf, int x, int y, tileset set, int idx, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(set->tw/2);
    int b = y-mode*(set->th/2);
    int tx = set->tiles[idx].x;
    int ty = set->tiles[idx].y;
    int w = set->map.width;
    for (int j = 0; j < set->tw; j++) {
        for (int k = 0; k < set->th; k++) {
            putpix(buf, a+j, b+k, set->map.data[IDX(tx+j, ty+k, w)]);
        }
    }
    if (set->tiles[idx].hascoll)
        collrect(cbuf, x, y, x+set->tw, y+set->th);
}

void blittilemap(buffer buf, buffer cbuf, int x, int y, tileset set, tilemap map, enum DRAW_IMAGE_MODE mode) {
    int w = map.width;
    for (int j = 0; j < w; j++) {
        for (int k = 0; k < map.height; k++) {
            blittile(buf, cbuf, x+j*16, y+k*16, set, map.data[IDX(j, k, w)], mode);
        }
    }
}

void blitstr(buffer buf, int x, int y, image font, char str[], size_t len, enum DRAW_IMAGE_MODE mode) {
    for (int i = 0; i < len; i++) {
        blitsprite(buf, x+i*6-mode*6*len/2, y-mode*3, (str[i]-32)*6, 0, 6, 6, font, TOP_LEFT);
    }
}

void render(buffer buf) {
    for (int y = 0; y < buf->height; y += 2) {
        for (int x = 0; x < buf->width; x++) {
            printf("\033[48;2;%d;%d;%d;38;2;%d;%d;%dm▄", buf->data[IDX(x, y, buf->width)] >> 16 & 0xFF, buf->data[IDX(x, y, buf->width)] >> 8 & 0xFF, buf->data[IDX(x, y, buf->width)] & 0xFF, buf->data[IDX(x, y+1, buf->width)] >> 16 & 0xFF, buf->data[IDX(x, y+1, buf->width)] >> 8 & 0xFF, buf->data[IDX(x, y+1, buf->width)] & 0xFF);
        }
        printf("%s", "\033[0m\r\n");
    }
}