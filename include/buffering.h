#include <stdlib.h>
#include "engine.h"

#pragma once

typedef struct {
    int width, height;
    int *data;
} *buffer;

buffer createbuffer(int width, int height) {
    buffer buf = (buffer)malloc(sizeof(*buf));

    buf->width = width;
    buf->height = height;
    buf->data = (int*)malloc(width*height*sizeof(int));

    return buf;
}

void freebuffer(buffer buf) {
    free(buf);
}