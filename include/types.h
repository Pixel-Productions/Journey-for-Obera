#ifdef __linux__
    #include <X11/Xlib.h>
#elif defined(_WIN32)
    // Windows
#endif

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
    TILE_GRID = 1
};

struct _window;

typedef struct _window *window;

#ifdef __linux__
    struct _window {
        int px, py;
        bool mouse_down;

        bool input[255];

        Display *display;
        Window win;

        XEvent *event;
    };
#elif defined(_WIN32)

#endif

typedef struct {
    void *val;
    enum ASSET_TYPE type;
} asset;

typedef struct {
    color_t *data;
    int width, height;
} image;

typedef struct {
    int x, y;
    bool hascoll;
} tile;

typedef struct {
    image map;
    tile *tiles;
    int tc, tw, th, tox, toy, tsx, tsy;
} tileset;

typedef struct {
    int *data;
    int width, height;
} tilemap;

typedef struct {
    int x, y;
} vec2;

typedef struct {
    int x, y;
    int width, height;
} rect2;

typedef struct {
    bool *data;
    int width, height;
} shape;

vec2 vec2_init(int x, int y) {
    vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

rect2 rect2_init(int x, int y, int width, int height) {
    rect2 r;
    
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    return r;
}

struct _list_elem {
    void *val;
    int len;

    struct _list_elem *prev;
    struct _list_elem *next;
};

typedef struct _list_elem *list_elem;

typedef list_elem list;

struct _dict_elem {
    void *key;
    void *val;
    int len;

    struct _dict_elem *prev;
    struct _dict_elem *next;
};

typedef struct _dict_elem *dict_elem;

typedef dict_elem dict;

struct _object {
    enum OBJECT_TYPE type;

    list data;
    
    shape shp;
    bool can_collide;
    bool is_collider;

    vec2 pos;
    vec2 vel;

    void (*tick)(struct _object *obj);
};

typedef struct _object *object;

typedef void (*object_tick_func)(object obj);

typedef void (*object_destructor_func)(object obj);

struct _game {
    window win;

    int width, height;
    size_t bsize;

    color_t *pbuf;
    bool *cbuf;

    dict assets;
    list objects;

    bool input[255];

    void (*tick)(struct _game *gm);
};

typedef struct _game *game;

typedef void (*game_tick_func)(game gm);

#ifdef __cplusplus
}
#endif