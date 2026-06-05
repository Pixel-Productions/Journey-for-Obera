#include <stdbool.h>

#include "engine.h"

void clear();

void putpix(int buffer[], int x, int y, color_t color);

void blitcol(int buffer[], int minx, int miny, int maxx, int maxy, color_t color);
void blitimg(int buffer[], int x, int y, image img, enum DRAW_IMAGE_MODE mode);
void blitsprite(int buffer[], int x, int y, int ix, int iy, int iw, int ih, image img, enum DRAW_IMAGE_MODE mode);
void blittile(int buffer[], bool cbuf[], int x, int y, tileset set, int idx, enum DRAW_IMAGE_MODE mode);
void blittilemap(int buffer[], bool cbuf[], int x, int y, tileset set, tilemap map, enum DRAW_IMAGE_MODE mode);
void blitscreen(int buffer[], screen scr);
void render(int buffer[]);