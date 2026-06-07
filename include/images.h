#include <stdbool.h>

#include "engine.h"

image loadimage(char path[]);

tileset loadtileset(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, enum TILE_DEFINITION_MODE mode);
void tilesetadd(tileset set, int x, int y, bool hascoll);
void freetileset(tileset set);
tilemap loadtilemap(char path[]);