#ifndef __ENGINE
#include "engine.h"
#endif

void clear();

void putpix(int buffer[], int x, int y, color_t color);

void blitcol(int buffer[], int minx, int miny, int maxx, int maxy, color_t color);

void blitimg(int buffer[], int x, int y, struct image img, int mode);

void blitsprite(int buffer[], int x, int y, int ix, int iy, int iw, int ih, struct image img, int mode);

void blittile(int buffer[], int x, int y, struct tilemap map, int idx, int mode);