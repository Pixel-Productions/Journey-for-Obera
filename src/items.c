#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "items.h"
#include "images.h"

int itc = 0;

item loaditem(char name[], size_t len) {
    item itm = (item)malloc(sizeof(*itm));
    itm->id = itc;
    itm->name = name;
    
    char pth[27+len];
    sprintf(pth, "assets/textures/items/%s.bmp", name);
    itm->texture = loadimage(pth);

    itc++;

    return itm;
}

void freeitem(item itm) {
    freeimage(itm->texture);
}