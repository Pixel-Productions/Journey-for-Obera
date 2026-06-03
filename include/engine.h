#define __ENGINE

typedef unsigned int color_t;

#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64

struct vec2 {
    int x, y;
};

typedef struct {
    color_t *data;
    int width;
    int height;
} *image;

typedef struct {
    image map;
    struct vec2 *tiles;
    int tc, tw, th, tox, toy, tsx, tsy, mode;
} *tilemap;

typedef struct {
    int width, height;
    int *data;
} *tilemapdata;

enum DRAW_IMAGE_MODE {
    TOP_LEFT = 0,
    CENTERED = 1
};

enum TILE_DEFINITION_MODE {
    AUTOMATIC = 0,
    USER_DEFINED = 1
};

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define SIDX(x, y) (IDX(x, y, SCREEN_WIDTH))

#define COLOR(r, g, b, a) ((color_t)((a) << 24 | (r) << 16 | (g) << 8 | (b)))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) ((a)+((b)-(a))*(x))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))