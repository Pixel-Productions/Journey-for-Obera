#include <stdlib.h>
#include "buffering.h"

buffer createbuffer(int width, int height) {
    buffer buf = (buffer)malloc(sizeof(*buf));

    buf->width = width;
    buf->height = height;
    buf->data = (color_t*)malloc(width*height*sizeof(color_t));

    return buf;
}

void freebuffer(buffer buf) {
    free(buf);
}