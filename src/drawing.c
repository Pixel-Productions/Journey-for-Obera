#include <stdio.h>
#include "drawing.h"
#include "physics.h"

void clear() {
    printf("%s", "\033[H");
}

void putpix(int buffer[], int x, int y, int xb, color_t color) {
    if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
        float a = (color >> 24 & 0xFF) / 255.0f;
        int r2 = color >> 16 & 0xFF;
        int g2 = color >> 8 & 0xFF;
        int b2 = color & 0xFF;
        
        int r1 = buffer[SIDX(x, y)] >> 16 & 0xFF;
        int g1 = buffer[SIDX(x, y)] >> 8 & 0xFF;
        int b1 = buffer[SIDX(x, y)] & 0xFF;

        color_t final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

        buffer[IDX(x, y, xb)] = final;
    }
}

void sputpix(int buffer[], int x, int y, color_t color) {
    putpix(buffer, x, y, SCREEN_WIDTH, color);
}

void blitcol(int buffer[], int minx, int miny, int maxx, int maxy, color_t color) {
    for (int x = minx; x < maxx; x++) {
        for (int y = miny; y < maxy; y++) {
            sputpix(buffer, x, y, color);
        }
    }
}

void blitimg(int buffer[], int x, int y, image img, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(img.width/2);
    int b = y-mode*(img.height/2);
    int w = img.width;
    for (int j = 0; j < img.width; j++) {
        for (int k = 0; k < img.height; k++) {
            sputpix(buffer, a+j, b+k, img.data[IDX(j, k, w)]);
        }
    }
}

void blitsprite(int buffer[], int x, int y, int ix, int iy, int iw, int ih, image img, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(iw/2);
    int b = y-mode*(ih/2);
    for (int j = 0; j < iw; j++) {
        for (int k = 0; k < ih; k++) {
            sputpix(buffer, a+j, b+k, img.data[IDX(j+ix, k+iy, img.width)]);
        }
    }
}

void blittile(int buffer[], bool cbuf[], int x, int y, tileset set, int idx, enum DRAW_IMAGE_MODE mode) {
    int a = x-mode*(set->tw/2);
    int b = y-mode*(set->th/2);
    int tx = set->tiles[idx].x;
    int ty = set->tiles[idx].y;
    int w = set->map.width;
    for (int j = 0; j < set->tw; j++) {
        for (int k = 0; k < set->th; k++) {
            sputpix(buffer, a+j, b+k, set->map.data[IDX(tx+j, ty+k, w)]);
        }
    }
    if (set->tiles[idx].hascoll)
        collrect(cbuf, x, y, x+set->tw, y+set->th);
}

void blittilemap(int buffer[], bool cbuf[], int x, int y, tileset set, tilemap map, enum DRAW_IMAGE_MODE mode) {
    int w = map.width;
    for (int j = 0; j < w; j++) {
        for (int k = 0; k < map.height; k++) {
            blittile(buffer, cbuf, x+j*16, y+k*16, set, map.data[IDX(j, k, w)], mode);
        }
    }
}

void blitscreen(int buffer[], screen scr) {
    for (int i = 0; i < scr.ec; i++) {
        uielem elem = scr.elems[i];

        for (int x = 0; x < elem.width; x++) {
            for (int y = 0; y < elem.height; y++) {
                sputpix(buffer, elem.x+x, elem.y+y, elem.data[IDX(elem.x+x, elem.y+y, elem.width)]);
            }
        }
    }
}

void blitstr(int buffer[], int x, int y, image font, char str[], size_t len, enum DRAW_IMAGE_MODE mode) {
    for (int i = 0; i < len; i++) {
        blitsprite(buffer, x+i*6-mode*6*len/2, y-mode*3, (str[i]-32)*6, 0, 6, 6, font, TOP_LEFT);
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