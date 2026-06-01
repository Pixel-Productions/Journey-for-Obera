#define __ENGINE

typedef unsigned int color_t;

#define SCREEN_WIDTH 100
#define SCREEN_HEIGHT 60

struct vec2 {
    int x, y;
};

struct image {
    color_t *data;
    int width;
    int height;
};

enum DRAW_IMAGE_MODE {
    TOP_LEFT,
    CENTERED
};

struct tilemap {
    struct image map;
};

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define SIDX(x, y) (IDX(x, y, SCREEN_WIDTH))

#define COLOR(r, g, b, a) ((color_t)(a << 24 | r << 16 | g << 8 | b))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) (a+(b-a)*x)

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))