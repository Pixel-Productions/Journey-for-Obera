#ifndef __ENGINE
#include "engine.h"
#endif

image loadimage(char path[]);
void freeimage(image img);

tilemap loadtilemap(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, int mode);
void tilemapadd(tilemap map, int x, int y);
tilemapdata loadlayout(char path[]);
void freetilemap(tilemap map);