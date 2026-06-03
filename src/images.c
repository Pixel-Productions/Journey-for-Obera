#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include "images.h"

image loadimage(char path[]) {
    image img = (image)malloc(sizeof(*img));

    FILE *file = fopen(path, "rb");

    unsigned char header[54];
    fread(header, 1, 54, file);

    img->width = header[18];
    img->height = abs(header[22]);

    int size = img->width*img->height;

    bool ud = header[22] > 0;

    int *raw = (int*)malloc(size*sizeof(int));
    fseek(file, header[10], SEEK_SET);
    fread(raw, sizeof(int), size*sizeof(int), file);
    
    color_t *data = (color_t*)malloc(size*sizeof(color_t));
    for (int x = 0; x < img->width; x++) {
        for (int y = 0; y < img->height; y++) {
            int i = IDX(x, y, img->width);
            data[(ud ? IDX(x, img->height-y-1, img->width) : i)] = COLOR(raw[i] >> 16, raw[i] >> 8, raw[i], raw[i] >> 24);
        }
    }

    img->data = data;

    fclose(file);

    return img;
}

void freeimage(image img) {
    free(img->data);
}

tilemap loadtilemap(char path[], int tw, int th, int tox, int toy, int tsx, int tsy, int mode) {
    tilemap map = (tilemap)malloc(sizeof(*map));
    map->map = loadimage(path);
    map->tw = tw;
    map->th = th;
    map->tox = tox;
    map->toy = toy;
    map->tsx = tsx;
    map->tsy = tsy;
    
    if (mode == AUTOMATIC) {
        int x = (map->map->width-tox)/(tw+tsx);
        int y = (map->map->height-toy)/(th+tsy);

        map->tiles = (struct vec2*)malloc(x*y*sizeof(struct vec2));

        for (int j = 0; j < x; j++) {
            for (int k = 0; k < y; k++) {
                struct vec2 tile;
                tile.x = j*tw+tox+tsx;
                tile.y = k*th+toy+tsy;
                map->tiles[IDX(j, k, x)];
            }
        }

        map->tc = x*y;
    } else {
        map->tiles = (struct vec2*)malloc(0);
        map->tc = 0;
    }

    return map;
}

void tilemapadd(tilemap map, int x, int y) {
    map->tiles = (struct vec2*)realloc(map->tiles, (map->tc+1)*sizeof(struct vec2));

    struct vec2 nw;
    nw.x = x*(map->tw+map->tsx)+map->tox;
    nw.y = y*(map->th+map->tsx)+map->toy;

    map->tiles[map->tc] = nw;
    map->tc = map->tc+1;
}

tilemapdata loadlayout(char path[]) {
    tilemapdata tmdat = (tilemapdata)malloc(sizeof(*tmdat));

    FILE *file = fopen(path, "r");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);

    unsigned char *data = (unsigned char*)malloc(len);
    fread(data, 1, len, file);
    fclose(file);

    tmdat->width = data[0];
    tmdat->height = data[1];

    tmdat->data = (int*)malloc(tmdat->width*tmdat->height*sizeof(int));

    for (int i = 0; i < tmdat->width*tmdat->height; i++) {
        tmdat->data[i] = data[i+2];
    }

    return tmdat;
}

void freetilemap(tilemap map) {
    free(map->map->data);
    free(map->tiles);
}