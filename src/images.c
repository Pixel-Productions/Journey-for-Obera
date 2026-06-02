#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "images.h"

struct image loadimage(char path[]) {
    struct image img;

    FILE *file = fopen(path, "rb");

    unsigned char header[54];
    fread(header, 1, 54, file);

    img.width = header[18];
    img.height = abs(header[22]);

    int size = img.width*img.height;

    bool ud = header[22] > 0;

    int *raw = (int*)malloc(size*sizeof(int));
    fseek(file, header[10], SEEK_SET);
    fread(raw, sizeof(int), size*sizeof(int), file);
    
    color_t *data = (color_t*)malloc(size*sizeof(color_t));
    for (int x = 0; x < img.width; x++) {
        for (int y = 0; y < img.height; y++) {
            int i = IDX(x, y, img.width);
            data[(ud ? IDX(x, img.height-y-1, img.width) : i)] = COLOR(raw[i] >> 16, raw[i] >> 8, raw[i], raw[i] >> 24);
        }
    }

    img.data = data;

    fclose(file);

    return img;
}

void freeimage(struct image img) {
    free(img.data);
}

struct tilemap loadtilemap(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, int mode) {
    struct tilemap map;
    map.map = loadimage(path);
    map.tw = tw;
    map.th = th;
    map.tox = tox;
    map.toy = toy;
    map.tsx = tsx;
    map.tsy = tsy;
    
    if (mode == AUTOMATIC) {
        int x = (map.map.width-tox)/(tw+tsx);
        int y = (map.map.height-toy)/(th+tsy);

        map.tiles = (struct vec2*)malloc(x*y*sizeof(struct vec2));

        for (int j = 0; j < x; j++) {
            for (int k = 0; k < y; k++) {
                struct vec2 tile;
                tile.x = j*tw+tox+tsx;
                tile.y = k*th+toy+tsy;
                map.tiles[IDX(j, k, x)];
            }
        }
    } else
        map.tiles = (struct vec2*)malloc(0);

    return map;
}

void tilemapadd(struct tilemap map, int x, int y) {
    map.tiles = (struct vec2*)realloc(map.tiles, sizeof(map.tiles)+sizeof(struct vec2));

    struct vec2 nw;
    nw.x = x;
    nw.y = y;

    map.tiles[LENOF(map.tiles)] = nw;
}

void freetilemap(struct tilemap map) {
    free(map.map.data);
    free(map.tiles);
} 