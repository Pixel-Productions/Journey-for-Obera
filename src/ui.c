#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ui.h"
#include "drawing.h"
#include "buffering.h"

screen loadscreen(char path[]) {
    screen scr;
    scr.ec = 0;

    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char*)malloc(len);
    fread(data, 1, len, file);

    scr.elems = (uielem*)malloc(data[0]*sizeof(uielem));

    int i = 0;
    int mi = 1;
    while (mi < len) {
        uielem elem;
        elem.x = data[mi+1];
        elem.y = data[mi+2];

        int inc = 0;

        int type = data[mi];
        if (type == 0) {
            int mx = data[mi+3];
            int my = data[mi+4];
        
            int xs = mx-elem.x;
            int ys = my-elem.y;

            elem.width = xs;
            elem.height = ys;

            elem.data = (color_t*)malloc(xs*ys*sizeof(color_t));
            
            color_t col = COLOR(data[mi+5], data[mi+6], data[mi+7], data[mi+8]);
            for (int x = 0; x < xs; x++) {
                for (int y = 0; y < ys; y++) {
                    elem.data[IDX(x, y, elem.width)] = col;
                }
            }
            
            inc = 9;
        } else if (type == 1) {
            int plen = 0;

            while (data[mi+3+plen] != 0) {
                plen++;
            }
            plen++;

            char pth[plen];
            memcpy(pth, data+mi+3, plen);

            image img = loadimage(pth);

            elem.width = img.width;
            elem.height = img.height;

            elem.data = (color_t*)malloc(elem.width*elem.height*sizeof(color_t));

            memcpy(elem.data, img.data, img.width*img.height*sizeof(color_t));

            freeimage(img);

            inc = plen+3;
        }

        scr.elems[i] = elem;
        
        i++;
        mi += inc;
        scr.ec++;
    }

    return scr;
}

void freescreen(screen scr) {
    for (int i = 0; i < scr.ec; i++) {
        free(scr.elems[i].data);
    }
    free(scr.elems);
}