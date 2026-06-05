#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ui.h"

screen loadscreen(char path[]) {
    screen scr;
    scr.ec = 0;

    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char*)malloc(len);
    fread(data, 1, len, file);

    scr.elems = (uielem*)malloc(sizeof(uielem));

    int i = 0;
    int mi = 0;
    while (mi < len) {
        uielem elem;
        elem.x = data[mi];
        elem.y = data[mi+1];

        int inc = 0;

        switch (data[mi+2]) {
            case 0:
                int xs = data[mi+3]-elem.x;
                int ys = data[mi+4]-elem.y;

                elem.width = xs;
                elem.height = ys;

                elem.data = (int*)malloc(xs*ys*sizeof(int));
                memset(elem.data, COLOR(data[mi+5], data[mi+6], data[mi+7], data[mi+8]), xs*ys*sizeof(int));
                
                inc = 9;
                break;
            case 1:
                int pln = data[mi+3];

                char pth[pln];
                memcpy(pth, data+mi+4, pln);

                image img = loadimage(pth);
                elem.data = img->data;

                elem.width = img->width;
                elem.height = img->height;

                inc = pln+4;
                break;
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