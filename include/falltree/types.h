#include <linux/limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <GLFW/glfw3.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int color_t;

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))
#define COLOR(r, g, b, a) ((color_t)((r) << 24 | (g) << 16 | (b) << 8 | (a)))
#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))
#define LERP(a, b, x) ((a) + ((b) - (a)) * (x))
#define ILERP(a, b, x) (((x) - (a)) / (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))
#define ROUND_TO(a, x) ((a) / (x) * (x))
#define NORMALIZE(x) ((x) / abs(x))


enum OBJECT_TYPE {
    RECT = 0,
    SPRITE = 1,
    ANIMATED_SPRITE = 2,
    TILE_GRID = 3,
    TEXT = 4,
    AREA = 5,
    ORIGIN = 6,
    VIDEO_PLAYBACK = 7
};

enum ASSET_TYPE {
    IMAGE = 0,
    TILESET = 1,
    TILEMAP = 2,
    SHAPE = 3,
    FONT = 4,
    AUDIO = 5,
    VIDEO = 6
};

enum EVENT_PHASE {
    PRE = 0,
    POST = 1
};

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

    int tilec;
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
    image img;
    int width, height;
} *font;



typedef struct {
    int width, height, framec, usd;
    color_t **frames;
} *video;

typedef union {
    image image;
    tileset tileset;
    tilemap tilemap;
    shape shape;
    font font;
    //audio audio;
    //video video;
} asset_type;

typedef struct {
    enum ASSET_TYPE type_name;
    asset_type type;
} *asset;

typedef struct {
    int x, y;
} vec2;

typedef struct {
    vec2 pos, scale, offset;
    float rot;
} transform2;

typedef struct {
    int x, y, z;
} vec3;

typedef struct {
    vec3 pos, rot, scale, offset;
} transform3;

typedef struct _list_elem {
    void *val;

    struct _list_elem *next;
} *list_elem;

typedef struct {
    list_elem first;
    size_t len;
} *list;

typedef struct _dict_elem {
    void *key;
    void *val;

    struct _dict_elem *next;
} *dict_elem;

typedef struct {
    dict_elem first;
    size_t len;
} *dict;

typedef struct _stack_elem {
    void *val;
    struct _stack_elem *next;
} *stack_elem;

typedef struct {
    stack_elem head;
    stack_elem tail;
    size_t size;
} *stack;

typedef struct {
    list children;
} *node_tree;

typedef struct {
    list children;

    void *val;
} *node;

typedef struct {
    int x, y, button;
} mouse_state;

typedef struct {
    int brightness;
    int radius;

    color_t color;

    vec2 pos;
} *light;

typedef struct {
    int rem;

    image img;
    vec2 pos;
    vec2 linvel;
} *particle;

typedef struct {
    list objects;

    list lights;
    list particles;
} *scene;

typedef struct _game {
    GLFWwindow *win;

    mouse_state mouse;
    bool keyboard[302];

    int width, height, scale;
    size_t bsize;
    size_t scaled_size;

    char *title;

    color_t bgcol;

    color_t *pbuf;
    short *cbuf;

    pthread_t render_thread;
    pthread_mutex_t mutex;

    int active_scene;
    list scenes;

    stack free_queue;

    list assets;

    void (*tick)(struct _game*, enum EVENT_PHASE);
} *game;

typedef void (*game_tick_func)(game, enum EVENT_PHASE);

typedef struct {
    int width, height;
    color_t color;
} *rect;

typedef struct {
    image img;
} *sprite;

typedef struct {
    image img;
    int w, h, hframes, vframes, ox, oy, sx, sy, frame;
} *animated_sprite;

typedef struct {
    tileset set;
    tilemap map;
} *tile_grid;

typedef struct {
    font fnt;
    char *string;
    int width;
} *text;

typedef struct {
    int width, height;
    bool colliding;
} *area;

typedef struct {
    video vid;

    bool playing;
} *video_playback;

typedef union {
    rect rect;
    sprite sprite;
    animated_sprite animated_sprite;
    tile_grid tile_grid;
    text text;
    area area;
    video_playback video_playback;
} obj_type;

enum OBJECT_FLAG_TYPE {
    STATIC = 0,
    DYNAMIC = 1
};

typedef struct {
    void *val;
    enum OBJECT_FLAG_TYPE type;
} *object_flag;

struct _object {
    short uid;

    list flags;

    enum OBJECT_TYPE type_name;
    obj_type type;
    
    shape shp;
    bool can_collide;
    bool is_collider;

    transform2 transform;
    vec2 vel;

    int zidx;
    bool visible;

    list children;
    list lights;

    void (*tick)(game, struct _object*);
};

typedef struct _object *object;

typedef void (*object_tick_func)(game, object);

#ifdef __cplusplus
}
#endif