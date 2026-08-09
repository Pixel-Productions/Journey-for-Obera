#include <stdio.h>
#include <stdbool.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int color_t;

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define COLOR(r, g, b, a) ((color_t)((r) << 24 | (g) << 16 | (b) << 8 | (a)))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) ((a)+((b)-(a))*(x))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))

#define ROUND_TO(a, x) ((a) / (x) * (x))

#define VOID_PTR(val) ((void*)&val)

#define VOID_PTR_CAST(val, type) (*(type*)(val))

enum ASSET_TYPE {
    IMAGE = 0,
    TILESET = 1,
    TILEMAP = 2,
    SHAPE = 3
};

enum OBJECT_TYPE {
    CUSTOM = -1,
    SPRITE = 0,
    ANIMATED_SPRITE = 1,
    TILE_GRID = 2
};

enum WINDOW_TYPE {
    X11 = 0,
    WAYLAND = 1
};

typedef struct {
    void *val;
    enum ASSET_TYPE type;
} *asset;

typedef struct {
    color_t *data;
    int width, height;
} *image;

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
    int *data;
    int width, height;
} *tilemap;

typedef struct {
    bool *data;
    int width, height;
} *shape;

typedef struct {
    int x, y;
} vec2;

typedef struct {
    int x, y;
    int width, height;
} rect2;

vec2 vec2_init(int x, int y) {
    vec2 vec = {
        .x = x,
        .y = y
    };
    return vec;
}

rect2 rect2_init(int x, int y, int w, int h) {
    rect2 rect = {
        .x = x,
        .y = y,
        .width = w,
        .height = h
    };
    return rect;
}

typedef struct _list_elem {
    void *val;
    int len;

    struct _list_elem *prev;
    struct _list_elem *next;
} *list_elem;

typedef list_elem list;

typedef struct _dict_elem {
    void *key;
    void *val;
    int len;

    struct _dict_elem *prev;
    struct _dict_elem *next;
} *dict_elem;

typedef dict_elem dict;

typedef struct _stack_elem {
    void *val;
    
    struct _stack_elem *next;
    struct _stack_elem *head;
    struct _stack_elem *tail;
} *stack_elem;

typedef stack_elem stack;

typedef struct {
    int x, y, button;
} mouse_state;

typedef struct {
    int width, height;
    bool should_close;

    enum WINDOW_TYPE type;

    dict data;

    mouse_state mouse;
    bool keyboard[255];
} *window;

struct _game {
    window win;

    mouse_state mouse;
    bool keyboard[255];

    int width, height, scale;
    size_t bsize;
    size_t scaled_size;

    color_t *pbuf;
    bool *cbuf;

    color_t *wbuf;

    dict assets;
    list objects;

    void (*tick)(struct _game *gm);
};

typedef struct _game *game;

typedef void (*game_tick_func)(game gm);

struct _object {
    enum OBJECT_TYPE type;

    list data;
    
    shape shp;
    bool can_collide;
    bool is_collider;

    vec2 pos;
    vec2 vel;

    void (*tick)(game gm, struct _object *obj);
};

typedef struct _object *object;

typedef void (*object_tick_func)(game gm, object obj);

typedef void (*object_destructor_func)(object obj);

#ifdef __cplusplus
}
#endif