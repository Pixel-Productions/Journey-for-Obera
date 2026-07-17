#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <memory.h>
#include "windowing.h"
#include "audio.h"
#include "list.h"
#include "dict.h"
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static image load_image(char *path) {
    image img;

    FILE *file = fopen(path, "rb");

    unsigned char header[4];
    fread(header, 1, 4, file);

    img.width = header[1] << 8 | header[0];
    img.height = abs(header[3] << 8 | header[2]);

    int size = img.width*img.height;
    
    img.data = (color_t*)malloc(size*sizeof(color_t));
    fseek(file, 4, SEEK_SET);
    fread(img.data, sizeof(color_t), size, file);

    fclose(file);

    return img;
}

static void free_image(image img) {
    free(img.data);
}

static tileset load_tileset(char *path) {
    tileset set;
    
    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char data[len];
    fread(data, len, 1, file);
    fclose(file);

    int plen = data[0];
    char ipth[plen];
    memcpy(ipth, data+1, plen);

    set.map = load_image(ipth);
    set.tw = data[plen+1];
    set.th = data[plen+2];
    set.tox = data[plen+3];
    set.toy = data[plen+4];
    set.tsx = data[plen+5];
    set.tsy = data[plen+6];

    int tlen = data[plen+7];
    set.tiles = (tile*)malloc(tlen*sizeof(tile));
    for (int i = 0; i < tlen; i++) {
        tile tl;
        tl.x = data[plen+8+i*3]*set.tw;
        tl.y = data[plen+9+i*3]*set.th;
        tl.hascoll = data[plen+10+i*3];

        set.tiles[i] = tl;
    }

    return set;
}

static void free_tileset(tileset set) {
    free_image(set.map);
    free(set.tiles);
}

static tilemap load_tilemap(char *path) {
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

static void free_tilemap(tilemap map) {
    free(map.data);
}

static shape load_shape(char *path) {
    shape shp;

    FILE *file = fopen(path, "rb");

    unsigned char header[4];
    fread(header, 1, 4, file);

    shp.width = header[1] << 8 | header[0];
    shp.height = abs(header[3] << 8 | header[2]);

    int size = shp.width*shp.height;
    
    shp.data = (bool*)malloc(size);
    fseek(file, 4, SEEK_SET);
    fread(shp.data, sizeof(color_t), size, file);

    fclose(file);

    return shp;
}

static void free_shape(shape shp) {
    free(shp.data);
}

void load_asset(game gm, char *key, char *path, enum ASSET_TYPE type) {
    asset ast;
    ast.type = type;

    switch (type) {
        case IMAGE: {
            image *img = (image*)malloc(sizeof(image));
            *img = load_image(path);
            ast.val = VOID_PTR(img);
            break;
        }
        case TILEMAP: {
            tilemap *map = (tilemap*)malloc(sizeof(tilemap));
            *map = load_tilemap(path);
            ast.val = VOID_PTR(map);
            break;
        }
        case TILESET: {
            tileset *set = (tileset*)malloc(sizeof(tileset));
            *set = load_tileset(path);
            ast.val = VOID_PTR(set);
            break;
        }
        case SHAPE: {
            shape *shp = (shape*)malloc(sizeof(tilemap));
            *shp = load_shape(path);
            ast.val = VOID_PTR(shp);
            break;
        }
    }

    dict_add_elem(gm->assets, key, VOID_PTR(ast));
}

asset get_asset(game gm, char *key) {
    return VOID_PTR_CAST(dict_get_elem(gm->assets, key)->val, asset);
}

game init_game(char *title, int width, int height, int scale) {
    game gm = (game)malloc(sizeof(*gm));

    gm->win = create_window(title, width, height);

    //start_audio();

    gm->width = width;
    gm->height = height;

    gm->bsize = width*height;

    gm->pbuf = (color_t*)calloc(width*height, sizeof(color_t));
    gm->cbuf = (bool*)calloc(width*height, 1);

    gm->objects = list_init();
    gm->assets = dict_init();

    return gm;
}

void put_pixel(game gm, int x, int y, color_t color) {
    if (x >= 0 && x < gm->width && y >= 0 && y < gm->height) {
        float a = (color >> 24 & 0xFF) / 255.0f;
        int r2 = color >> 16 & 0xFF;
        int g2 = color >> 8 & 0xFF;
        int b2 = color & 0xFF;
        
        int r1 = gm->pbuf[IDX(x, y, gm->width)] >> 16 & 0xFF;
        int g1 = gm->pbuf[IDX(x, y, gm->width)] >> 8 & 0xFF;
        int b1 = gm->pbuf[IDX(x, y, gm->width)] & 0xFF;

        int final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

        gm->pbuf[IDX(x, y, gm->width)] = final;
    }
}

void blit_image(game gm, int x, int y, image img) {
    for (int j = 0; j < img.width; j++) {
        for (int k = 0; k < img.height; k++) {
            put_pixel(gm, x+j, y+k, img.data[IDX(x, y, gm->width)]);
        }
    }
}

void blit_shape(game gm, int x, int y, shape shp) {
    for (int j = 0; j < shp.width; j++) {
        for (int k = 0; k < shp.height; k++) {
            gm->cbuf[IDX(x+j, y+k, gm->width)] = shp.data[IDX(x, y, shp.width)];
        }
    }
}

void game_start(game gm) {
    while (!glfwWindowShouldClose(gm->win)) {
        glfwPollEvents();

        // Clear collision buffer
        memset(gm->cbuf, false, gm->bsize);

        for (int i = 0; i < gm->objects->len; i++) {
            object obj = (object)list_get_elem(gm->objects, i)->val;

            if (obj->tick != NULL)
                obj->tick(obj);

            if (obj->is_collider)
                blit_shape(gm, obj->pos.x, obj->pos.y, obj->shp);
        }

        for (int i = 0; i < gm->objects->len; i++) {
            object obj = (object)list_get_elem(gm->objects, i)->val;

            switch (obj->type) {
                case SPRITE: {
                    blit_image(gm, obj->pos.x, obj->pos.y, VOID_PTR_CAST(list_get_elem(obj->data, 0), image));
                    break;
                }
                case TILE_GRID: {
                    tileset set = VOID_PTR_CAST(list_get_elem(obj->data, 0), tileset);
                    tilemap map = VOID_PTR_CAST(list_get_elem(obj->data, 1), tilemap);

                    for (int x = 0; x < map.width; x++) {
                        for (int y = 0; y < map.height; y++) {
                            tile tl = set.tiles[map.data[IDX(x, y, map.width)]];
                            for (int j = 0; j < set.tw; j++) {
                                for (int k = 0; k < set.th; k++) {
                                    put_pixel(gm, obj->pos.x+set.tw, obj->pos.y+set.tw, set.map.data[IDX(tl.x+j, tl.y+k, set.map.width)]);
                                }
                            }
                        }
                    }

                    break;
                }
            }
        }
        
        glDrawPixels(gm->width, gm->height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, gm->pbuf);

        glfwSwapBuffers(gm->win);

        // Add ~30fps delay
        usleep(33333);
    }
}

void end_game(game gm) {
    for (int i = 0; i < gm->objects->len; i++) {
        object obj = (object)list_get_elem(gm->objects, i)->val;

        switch (obj->type) {
            case CUSTOM: {
                VOID_PTR_CAST(list_get_elem(obj->data, 1), object_destructor_func)(obj);
            }
            case SPRITE: {}
            case TILE_GRID: {}
        }
    }

    for (int i = 0; i < gm->assets->len; i++) {
        asset ast = VOID_PTR_CAST(dict_get_elem_idx(gm->assets, i)->val, asset);

        switch (ast.type) {
            case IMAGE: {
                free_image(VOID_PTR_CAST(ast.val, image));
            }
            case TILESET: {
                tileset set = VOID_PTR_CAST(ast.val, tileset);

                free_image(set.map);
                free(set.tiles);
            }
            case TILEMAP: {
                free(VOID_PTR_CAST(ast.val, tilemap).data);
            }
        }
    }

    free(gm->pbuf);
    free(gm->cbuf);

    glfwDestroyWindow(gm->win);
    glfwTerminate();

    //end_audio();

    free(gm);
}

#pragma endregion

#ifdef __cplusplus
}
#endif