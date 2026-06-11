#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "engine.h"

#pragma once

enum DRAW_IMAGE_MODE {
    TOP_LEFT = 0,
    CENTERED = 1
};

enum TILE_DEFINITION_MODE {
    AUTOMATIC = 0,
    USER_DEFINED = 1
};

typedef struct {
    color_t *data;
    int width;
    int height;
} image;

typedef struct {
    int x, y;
    bool hascoll;
} tile;

typedef struct {
    image map;
    tile *tiles;
    int tc, tw, th, tox, toy, tsx, tsy;
} *tileset;

typedef struct {
    int width, height;
    int *data;
} tilemap;

image loadimage(char path[]) {
    image img;

    FILE *file = fopen(path, "rb");

    unsigned char header[54];
    fread(header, 1, 54, file);

    img.width = header[19] << 8 | header[18];
    img.height = abs(header[23] << 8 | header[22]);

    int size = img.width*img.height;

    bool ud = header[22] > 0;

    int *raw = (int*)malloc(size*sizeof(int));
    fseek(file, header[10], SEEK_SET);
    fread(raw, sizeof(int), size, file);
    
    img.data = (color_t*)malloc(size*sizeof(color_t));

    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            int i = IDX(x, y, img.width);
            img.data[(ud ? IDX(x, img.height-y-1, img.width) : i)] = COLOR(raw[i] >> 16, raw[i] >> 8, raw[i], raw[i] >> 24);
        }
    }

    fclose(file);

    return img;
}

void freeimage(image img) {
    free(img.data);
}

tileset loadtileset(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, enum TILE_DEFINITION_MODE mode) {
    tileset set = (tileset)malloc(sizeof(*set));
    set->map = loadimage(path);
    set->tw = tw;
    set->th = th;
    set->tox = tox;
    set->toy = toy;
    set->tsx = tsx;
    set->tsy = tsy;
    
    if (mode == AUTOMATIC) {
        int x = (set->map.width-tox)/(tw+tsx);
        int y = (set->map.height-toy)/(th+tsy);

        set->tiles = (tile*)malloc(x*y*sizeof(tile));

        for (int j = 0; j < x; j++) {
            for (int k = 0; k < y; k++) {
                tile nw;
                nw.x = j*tw+tox+tsx;
                nw.y = k*th+toy+tsy;
                nw.hascoll = false;
                set->tiles[IDX(j, k, x)];
            }
        }

        set->tc = x*y;
    } else {
        set->tiles = (tile*)malloc(0);
        set->tc = 0;
    }

    return set;
}

void tilesetadd(tileset set, int x, int y, bool hascoll) {
    set->tiles = (tile*)realloc(set->tiles, (set->tc+1)*sizeof(tile));

    tile nw;
    nw.x = x*(set->tw+set->tsx)+set->tox;
    nw.y = y*(set->th+set->tsx)+set->toy;
    nw.hascoll = hascoll;

    set->tiles[set->tc] = nw;
    set->tc = set->tc+1;
}

void freetileset(tileset set) {
    free(set);
}

tilemap loadtilemap(char path[]) {
    tilemap map;

    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char*)malloc(len);
    fread(data, 1, len, file);
    fclose(file);

    map.width = data[0];
    map.height = data[1];

    map.data = (int*)malloc(map.width*map.height*sizeof(int));

    for (int i = 0; i < map.width*map.height; i++) {
        map.data[i] = data[i+2];
    }

    return map;
}

void freetilemap(tilemap map) {
    free(map.data);
}