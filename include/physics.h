#include <stdbool.h>

#ifndef __ENGINE
#include "engine.h"
#endif

void collrect(bool buffer[], int minx, int miny, int maxx, int maxy);
void moveandcoll(bool buffer[], vec2 vel, vec2 *pos);