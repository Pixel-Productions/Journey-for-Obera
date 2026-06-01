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

struct tilemap loadtilemap(char path[], int tox, int toy, int tsx, int tsy, int tw, int th) {
    struct tilemap tm;

    for (int x = 0; x < tw; x++) {
        for (int y = 0; y < th; y++) {

        }
    }
}