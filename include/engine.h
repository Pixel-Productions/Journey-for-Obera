#include <stdbool.h>

#ifndef __ENGINE
#define __ENGINE

typedef unsigned int color_t;

#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64

enum DRAW_IMAGE_MODE {
    TOP_LEFT = 0,
    CENTERED = 1
};

enum TILE_DEFINITION_MODE {
    AUTOMATIC = 0,
    USER_DEFINED = 1
};

enum ITEM_TYPE {
    BASIC = 0,
    FOOD = 1
};

typedef struct {
    int x, y;
} vec2;

typedef struct {
    color_t *data;
    int width;
    int height;
} *image;

typedef struct {
    int x, y;
    bool hascoll;
} tile;

typedef struct {
    image map;
    tile *tiles;
    int tc, tw, th, tox, toy, tsx, tsy;
    enum TILE_DEFINITION_MODE mode;
} *tileset;

typedef struct {
    int width, height;
    int *data;
} *tilemap;


typedef struct {
    int x, y, width, height;
    int *data;
} uielem;

typedef struct {
    int width, height;
    uielem *elems;
    int ec;
} screen;


typedef struct {
    int id;
    char *name;
    image texture;
    enum ITEM_TYPE type;
    int *data;
} *item;


#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define SIDX(x, y) (IDX(x, y, SCREEN_WIDTH))

#define COLOR(r, g, b, a) ((color_t)((a) << 24 | (r) << 16 | (g) << 8 | (b)))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) ((a)+((b)-(a))*(x))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))

#endif