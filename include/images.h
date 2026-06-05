#include <stdbool.h>

#ifndef __ENGINE
#include "engine.h"
#endif

image loadimage(char path[]);
void freeimage(image img);

tileset loadtileset(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, enum TILE_DEFINITION_MODE mode);
void tilesetadd(tileset set, int x, int y, bool hascoll);
tilemap loadtilemap(char path[]);
void freetileset(tileset set);
void freetilemap(tilemap map);