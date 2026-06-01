#ifndef __ENGINE
#include "engine.h"
#endif

struct image loadimage(char path[]);

void freeimage(struct image img);

struct tilemap loadtilemap(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, int mode);

void tilemapadd(struct tilemap map, int x, int y);

void freetilemap(struct tilemap map);