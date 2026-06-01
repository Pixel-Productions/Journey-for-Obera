#ifndef __ENGINE
#include "engine.h"
#endif

struct image loadimage(char path[]);

struct image loadsampleimage(char path[], int x, int y, int w, int h);

void freeimage();

struct tilemap loadtilemap(char path[]);

void freetilemap();