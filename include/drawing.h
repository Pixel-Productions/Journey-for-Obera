#include <stdbool.h>

#include "engine.h"

void clear();

void putpix(buffer buf, int x, int y, color_t color);

void blitcol(buffer buf, int minx, int miny, int maxx, int maxy, color_t color);
void blitimg(buffer buf, int x, int y, image img, enum DRAW_IMAGE_MODE mode);
void blitsprite(buffer buf, int x, int y, int ix, int iy, int iw, int ih, image img, enum DRAW_IMAGE_MODE mode);
void blittile(buffer buf, bool cbuf[], int x, int y, tileset set, int idx, enum DRAW_IMAGE_MODE mode);
void blittilemap(buffer buf, bool cbuf[], int x, int y, tileset set, tilemap map, enum DRAW_IMAGE_MODE mode);
void blitscreen(buffer buf, screen scr);
void blitstr(buffer buf, int x, int y, image font, char str[], size_t len, enum DRAW_IMAGE_MODE mode);

void render(buffer buf);