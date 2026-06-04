#include <stdio.h>
#include "drawing.h"

void clear() {
    printf("%s", "\033[H");
}

void putpix(int buffer[], int x, int y, color_t color) {
    if (x >= 0 && x <= SCREEN_WIDTH && y >= 0 && y <= SCREEN_HEIGHT) {
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

void blittile(int buffer[], int x, int y, tileset set, int idx, int mode) {
    int a = x-mode*(set->tw/2);
    int b = y-mode*(set->th/2);
    int tx = set->tiles[idx].x;
    int ty = set->tiles[idx].y;
    int w = set->map->width;
    for (int j = 0; j < set->tw; j++) {
        for (int k = 0; k < set->th; k++) {
            putpix(buffer, a+j, b+k, set->map->data[IDX(tx+j, ty+k, w)]);
        }
    }
}

void blittilemap(int buffer[], int x, int y, tileset set, tilemap map, int mode) {
    int w = map->width;
    for (int j = 0; j < w; j++) {
        for (int k = 0; k < map->height; k++) {
            blittile(buffer, x+j*16, y+k*16, set, map->data[IDX(j, k, w)], mode);
        }
    }
}

void render(int buffer[]) {
    for (int y = 0; y < SCREEN_HEIGHT; y += 2) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            printf("\033[48;2;%d;%d;%d;38;2;%d;%d;%dm▄", buffer[SIDX(x, y)] >> 16 & 0xFF, buffer[SIDX(x, y)] >> 8 & 0xFF, buffer[SIDX(x, y)] & 0xFF, buffer[SIDX(x, y+1)] >> 16 & 0xFF, buffer[SIDX(x, y+1)] >> 8 & 0xFF, buffer[SIDX(x, y+1)] & 0xFF);
        }
        printf("%s", "\033[0m\r\n");
    }
}