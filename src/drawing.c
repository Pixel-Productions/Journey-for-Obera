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

void blitimg(int buffer[], int x, int y, struct image img, int mode) {
    for (int j = 0; j < img.width; j++) {
        for (int k = 0; k < img.height; k++) {
            putpix(buffer, x+j-mode*(img.width/2), y+k-mode*(img.height/2), img.data[IDX(j, k, img.width)]);
        }
    }
}

void blitsprite(int buffer[], int x, int y, int ix, int iy, int iw, int ih, struct image img, int mode) {
    for (int j = 0; j < iw; j++) {
        for (int k = 0; k < ih; k++) {
            putpix(buffer, x+j-mode*(iw/2), y+k-mode*(ih/2), img.data[IDX(j+ix, k+iy, iw)]);
        }
    }
}