#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "ui.h"
#include "drawing.h"
#include "assets.h"

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

        int type = data[mi+2];
        if (type == 0) {
            int mx = data[mi+3];
            int my = data[mi+4];
        
            int xs = mx-elem.x;
            int ys = my-elem.y;

            elem.width = xs;
            elem.height = ys;

            elem.data = (color_t*)malloc(xs*ys*sizeof(color_t));
            
            color_t col = COLOR(data[mi+5], data[mi+6], data[mi+7], data[mi+8]);
            for (int x = elem.x; x < mx; x++) {
                for (int y = elem.y; y < my; y++) {
                    elem.data[IDX(x, y, elem.width)] = col;
                }
            }
            
            inc = 9;
        } else if (type == 1) {
            int pln = data[mi+3];

            char pth[pln];
            memcpy(pth, data+mi+4, pln);

            image img = loadimage(pth);
            elem.data = img.data;

            elem.width = img.width;
            elem.height = img.height;

            inc = pln+4;
        } else if (type == 2) {
            int sln = data[mi+3];

            char str[sln];
            memcpy(str, data+mi+4, sln);

            color_t txt[sln*36];

            for (int i = 0; i < len; i++) {
                for (int j = 0; j < 6; j++) {
                    for (int k = 0; k < 6; k++) {
                        putpix(txt, j+i*6, k, sln*6, font_default.data[IDX(j+(str[i]-32)*6, k, font_default.width)]);
                    }
                }
            }

            elem.data = txt;
            elem.width = sln*6;
            elem.height = 6;

            inc = sln+4;
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