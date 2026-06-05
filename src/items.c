#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "items.h"
#include "images.h"

int itc = 0;

item loaditem(char dpth[], char tpth[]) {
    item itm = (item)malloc(sizeof(*itm));
    itm->id = itc;

    FILE* dfl = fopen(dpth, "rb");

    fseek(dfl, 0, SEEK_END);
    int len = ftell(dfl);
    rewind(dfl);

    unsigned char data[len];
    fread(data, 1, len, dfl);

    int nlen = data[0];

    itm->name = (char*)malloc(nlen);
    memcpy(itm->name, data+1, nlen);
    
    itm->texture = loadimage(tpth);

    itm->type = data[nlen];

    switch (data[nlen]) {
        case 0:
            itm->data = (int*)malloc(0);
            break;
        case 1:
            itm->data = (int*)malloc(sizeof(int));
            itm->data[0] = data[nlen+1];
            break;
    }

    itc++;

    return itm;
}

void freeitem(item itm) {
    free(itm->name);
    freeimage(itm->texture);
}