#include <stdbool.h>

#pragma once

typedef unsigned int color_t;

typedef struct {
    int x, y;
} vec2;

typedef struct {
    int x, y;
    int width, height;
} rect2;

vec2 newvec2(int x, int y) {
    vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

rect2 newrect2(int x, int y, int width, int height) {
    rect2 r;
    
    r.x = x;
    r.y = y;
    r.width = width;
    r.height = height;
    return r;
}

#define PI 3.1415

#define IDX(x, y, xb) ((x) + (y) * (xb))

#define COLOR(r, g, b, a) ((color_t)((a) << 24 | (r) << 16 | (g) << 8 | (b)))

#define LENOF(arr) (sizeof(arr) / sizeof(arr[0]))

#define LERP(a, b, x) ((a)+((b)-(a))*(x))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define CLAMP(a, x, y) (MIN(MAX((a), (x)), (y)))