#define STB_IMAGE_IMPLEMENTATION

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
#include "stb/stb_image.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static image load_image(FILE *file) {
    image img = (image)malloc(sizeof(*img));

    int channels;
    unsigned char *data = stbi_load_from_file(file, &img->width, &img->height, &channels, STBI_rgb_alpha);
    img->data = (color_t*)malloc(img->width*img->height*sizeof(color_t));

    for (int i = 0; i < img->width*img->height; i++) {
        img->data[i] = COLOR(data[i*4], data[i*4 + 1], data[i*4 + 2], data[i*4 + 3]);
    }

    return img;
}

static tileset load_tileset(FILE *file) {
    tileset set = (tileset)malloc(sizeof(*set));

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char data[len];
    fread(data, len, 1, file);

    int plen = data[0];
    char ipth[plen];
    memcpy(ipth, data+1, plen);

    FILE *img = fopen(ipth, "rb");
    set->map = load_image(img);
    fclose(img);

    set->tw = data[plen+1];
    set->th = data[plen+2];
    set->tox = data[plen+3];
    set->toy = data[plen+4];
    set->tsx = data[plen+5];
    set->tsy = data[plen+6];

    int tlen = data[plen+7];
    set->tiles = (tile*)malloc(tlen*sizeof(tile));
    for (int i = 0; i < tlen; i++) {
        tile tl;
        tl.x = data[plen+8+i*3]*set->tw;
        tl.y = data[plen+9+i*3]*set->th;
        tl.hascoll = data[plen+10+i*3];

        set->tiles[i] = tl;
    }

    return set;
}

static tilemap load_tilemap(FILE *file) {
    tilemap map = (tilemap)malloc(sizeof(*map));

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char*)malloc(len);
    fread(data, 1, len, file);

    map->width = data[0];
    map->height = data[1];

    map->data = (int*)malloc(map->width*map->height*sizeof(int));

    for (int i = 0; i < map->width*map->height; i++) {
        map->data[i] = data[i+2];
    }

    return map;
}

static shape load_shape(FILE *file) {
    shape shp = (shape)malloc(sizeof(*shp));

    unsigned char header[4];
    fread(header, 1, 4, file);

    shp->width = header[1] << 8 | header[0];
    shp->height = abs(header[3] << 8 | header[2]);

    int size = shp->width*shp->height;
    
    shp->data = (bool*)malloc(size);
    fseek(file, 4, SEEK_SET);
    fread(shp->data, 1, size, file);

    return shp;
}

void load_asset(game gm, char *key, char *path, enum ASSET_TYPE type) {
    asset ast = (asset)malloc(sizeof(*ast));
    ast->type = type;

    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: No such file or directory\n");
        exit(-1);
    }

    switch (type) {
        case IMAGE: {
            ast->val = (void*)load_image(file);
            break;
        }
        case TILESET: {
            ast->val = (void*)load_tileset(file);
            break;
        }
        case TILEMAP: {
            ast->val = (void*)load_tilemap(file);
            break;
        }
        case SHAPE: {
            ast->val = (void*)load_shape(file);
            break;
        }
    }

    fclose(file);

    dict_add_elem(gm->assets, key, (void*)ast);
}

asset get_asset(game gm, char *key) {
    return (asset)dict_get_elem(gm->assets, key)->val;
}

game game_init(char *title, int width, int height, int scale) {
    game gm = (game)malloc(sizeof(*gm));

    gm->win = window_init(title, width*scale, height*scale);

    //start_audio();

    gm->width = width;
    gm->height = height;
    gm->scale = scale;

    gm->bsize = width*height;
    gm->scaled_size = width*scale*height*scale;

    gm->pbuf = (color_t*)calloc(width*height, sizeof(color_t));
    gm->cbuf = (bool*)calloc(width*height, 1);
    gm->wbuf = (color_t*)calloc(width*scale*height*scale, sizeof(color_t));

    gm->objects = list_init();
    gm->assets = dict_init();

    return gm;
}

void put_pixel(game gm, int x, int y, color_t color) {
    if (x >= 0 && x < gm->width && y >= 0 && y < gm->height) {
        float a = (color & 0xFF) / 255.0f;
        int r2 = color >> 24 & 0xFF;
        int g2 = color >> 16 & 0xFF;
        int b2 = color >> 8 & 0xFF;
        
        int r1 = gm->pbuf[IDX(x, y, gm->width)] >> 24 & 0xFF;
        int g1 = gm->pbuf[IDX(x, y, gm->width)] >> 16 & 0xFF;
        int b1 = gm->pbuf[IDX(x, y, gm->width)] >> 8 & 0xFF;

        int final = COLOR((int)LERP(r1, r2, a), (int)LERP(g1, g2, a), (int)LERP(b1, b2, a), (int)(a*255));

        gm->pbuf[IDX(x, y, gm->width)] = final;
    }
}

void blit_image(game gm, int x, int y, image img) {
    for (int j = 0; j < img->width; j++) {
        for (int k = 0; k < img->height; k++) {
            put_pixel(gm, x+j, y+k, img->data[IDX(j, k, img->width)]);
        }
    }
}

void blit_shape(game gm, int x, int y, shape shp) {
    for (int j = 0; j < shp->width; j++) {
        for (int k = 0; k < shp->height; k++) {
            if (x+j >= 0 && x+j < gm->width && y+k >= 0 && y+k < gm->height)
                gm->cbuf[IDX(x+j, y+k, gm->width)] = shp->data[IDX(j, k, shp->width)];
        }
    }
}

void game_start(game gm) {
    while (!gm->win->should_close) {
        window_frame(gm->win);

        gm->mouse = gm->win->mouse;
        memcpy(gm->keyboard, gm->win->keyboard, 255);

        gm->tick(gm);

        memset(gm->pbuf, 0, gm->bsize*4);
        memset(gm->cbuf, false, gm->bsize);

        for (int i = 0; i < gm->objects->len; i++) {
            object obj = (object)list_get_elem(gm->objects, i)->val;

            if (obj->tick != NULL)
                obj->tick(gm, obj);

            if (obj->is_collider)
                blit_shape(gm, obj->pos.x, obj->pos.y, obj->shp);
        }

        for (int i = 0; i < gm->objects->len; i++) {
            object obj = (object)list_get_elem(gm->objects, i)->val;

            switch (obj->type) {
                case SPRITE: {
                    blit_image(gm, obj->pos.x, obj->pos.y, (image)list_get_elem(obj->data, 0)->val);
                    break;
                }
                case ANIMATED_SPRITE: {
                    image img = (image)list_get_elem(obj->data, 0)->val;

                    int w = VOID_PTR_CAST(list_get_elem(obj->data, 1)->val, int);
                    int h = VOID_PTR_CAST(list_get_elem(obj->data, 2)->val, int);
                    int hframes = VOID_PTR_CAST(list_get_elem(obj->data, 3)->val, int);
                    int vframes = VOID_PTR_CAST(list_get_elem(obj->data, 4)->val, int);
                    int sx = VOID_PTR_CAST(list_get_elem(obj->data, 5)->val, int);
                    int sy = VOID_PTR_CAST(list_get_elem(obj->data, 6)->val, int);
                    int ox = VOID_PTR_CAST(list_get_elem(obj->data, 7)->val, int);
                    int oy = VOID_PTR_CAST(list_get_elem(obj->data, 8)->val, int);

                    int frame = VOID_PTR_CAST(list_get_elem(obj->data, 9)->val, int);

                    int fx = frame % hframes;
                    int fy = frame / hframes;

                    for (int j = 0; j < w; j++) {
                        for (int k = 0; k < h; k++) {
                            put_pixel(gm, obj->pos.x+j, obj->pos.y+k, img->data[IDX(ox + fx*sx + fx*w + j, oy + fy*sy + fy*h + k, img->width)]);
                        }
                    }
                    
                    break;
                }
                case TILE_GRID: {
                    tileset set = VOID_PTR_CAST(list_get_elem(obj->data, 0), tileset);
                    tilemap map = VOID_PTR_CAST(list_get_elem(obj->data, 1), tilemap);

                    for (int x = 0; x < map->width; x++) {
                        for (int y = 0; y < map->height; y++) {
                            tile tl = set->tiles[map->data[IDX(x, y, map->width)]];
                            for (int j = 0; j < set->tw; j++) {
                                for (int k = 0; k < set->th; k++) {
                                    put_pixel(gm, obj->pos.x+set->tw+j, obj->pos.y+set->tw+k, set->map->data[IDX(tl.x+j, tl.y+k, set->map->width)]);
                                }
                            }
                        }
                    }

                    break;
                }
            }
        }

        for (int x = 0; x < gm->width*gm->scale; x++) {
            for (int y = 0; y < gm->height*gm->scale; y++) {
                gm->wbuf[IDX(x, y, gm->width*gm->scale)] = gm->pbuf[IDX(x/gm->scale, y/gm->scale, gm->width)];
            }
        }

        window_blit_pixels(gm->win, gm->wbuf);

        // Add ~30fps delay
        usleep(33333);
    }
}

void end_game(game gm) {
    for (int i = 0; i < gm->objects->len; i++) {
        object obj = (object)list_get_elem(gm->objects, i)->val;

        switch (obj->type) {
            case CUSTOM: {
                VOID_PTR_CAST(list_get_elem(obj->data, 0)->val, object_destructor_func)(obj);
                break;
            }
            case SPRITE: { break; }
            case ANIMATED_SPRITE: {
                free((int*)list_get_elem(obj->data, 1)->val);
                free((int*)list_get_elem(obj->data, 2)->val);
                free((int*)list_get_elem(obj->data, 3)->val);
                free((int*)list_get_elem(obj->data, 4)->val);
                free((int*)list_get_elem(obj->data, 5)->val);
                free((int*)list_get_elem(obj->data, 6)->val);
                free((int*)list_get_elem(obj->data, 7)->val);
                free((int*)list_get_elem(obj->data, 8)->val);
                free((int*)list_get_elem(obj->data, 9)->val);
                break;
            }
            case TILE_GRID: { break; }
        }

        free_list(obj->data);
    }

    for (int i = 0; i < gm->assets->len; i++) {
        asset ast = (asset)dict_get_elem_idx(gm->assets, i)->val;

        switch (ast->type) {
            case IMAGE: {
                image img = (image)ast->val;
                free(img->data);
                free(img);
                break;
            }
            case TILESET: {
                tileset set = (tileset)ast->val;
                free(set->map->data);
                free(set->map);
                free(set->tiles);
                free(set);
                break;
            }
            case TILEMAP: {
                tilemap map = (tilemap)ast->val;
                free(map->data);
                free(map);
                break;
            }
            case SHAPE: {
                shape shp = (shape)ast->val;
                free(shp->data);
                free(shp);
            }
        }

        free(ast);
    }

    free_list(gm->objects);
    free_dict(gm->assets);

    free(gm->pbuf);
    free(gm->cbuf);
    free(gm->wbuf);

    //end_audio();

    end_window(gm->win);

    free(gm);
}

#pragma endregion

#ifdef __cplusplus
}
#endif