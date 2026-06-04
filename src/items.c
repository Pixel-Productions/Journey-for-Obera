#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "items.h"
#include "images.h"

int itc = 0;

item loaditem(char path[]) {
    item itm = (item)malloc(sizeof(*itm));
    itm->id = itc;

    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char*)malloc(len);

    fread(data, 1, len, file);

    itm->name = (char*)malloc(data[0]);
    
    for (int i = 0; i < data[0]; i++) {
        itm->name[i] = data[i+1];
    }

    itc++;

    return itm;
}

void freeitem(item itm) {
    free(itm->name);
    //freeimage(itm->texture);
}