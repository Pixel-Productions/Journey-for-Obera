#include <sys/stat.h>
#include <sys/types.h>
#include "constants.h"
#include "room.h"

#pragma once

const char savenm[] = "save.jfo";
char savedir[500];
char savepth[512];

//              Meta  Inventory  Member Count  Members  (Member ID  Position       HP and PW  Spell Count  Spell IDs  )
#define SAVE_SZ 64+   12+        1+            3*       (1+         2*sizeof(int)+ 2+         1+           8          )

void init_savesys() {
    #ifdef __linux__
        sprintf(savedir, "%s/.local/share/jfo", getenv("HOME"));
    #elif defined(_WIN32)
        sprintf(savedir, "%s/AppData/Local/jfo", getenv("userprofile"));
    #endif

    sprintf(savepth, "%s/%s", savedir, savenm);
}


int getint(unsigned char *arr, int i) {
    return arr[i] | arr[i+1] << 8 | arr[i+2] << 16 | arr[i+3] << 24;
}

void putint(unsigned char *arr, int i, int val) {
    arr[i] = val;
    arr[i+1] = val >> 8 & 0xFF;
    arr[i+2] = val >> 16 & 0xFF;
    arr[i+3] = val >> 24 & 0xFF;
}

void save() {
    unsigned char data[SAVE_SZ];

    data[0] = current_room;

    data[1] = seen_intro;
    data[2] = seen_mother_greet;
    data[3] = seen_marrin_greet;
    data[4] = seen_ceremony;

    for (int i = 0; i < 12; i++) {
        data[65 + i] = inventory[i];
    }

    data[77] = party->len;
    for (int i = 0; i < party->len; i++) {
        member mem = (member)list_get_elem(party, i);

        data[78] = mem->id;

        putint(data, 79, mem->obj->transform.pos.x);
        putint(data, 83, mem->obj->transform.pos.y);

        data[84] = mem->hp;
        data[85] = mem->pw;
        
        data[86] = mem->spellc;
        for (int j = 0; j < mem->spellc; j++) {
            data[87 + j] = mem->spells[j];
        }
    }

    FILE *savefl = fopen(savepth, "wb");
    fwrite(data, 1, SAVE_SZ, savefl);
    fclose(savefl);
}

static member create_member(char id, char hp, char pw, object obj, char spellc, char *spells) {
    member mem = (member)malloc(sizeof(*mem));

    mem->id = id;

    mem->hp = hp;
    mem->pw = pw;
    
    mem->obj = obj;
    mem->spellc = spellc;
    memcpy(mem->spells, spells, 6);

    return mem;
}


// Get a character's object and apply an offset.
static object get_char_offset(unsigned char id, int x, int y) {
    object obj;

    switch (id) {
        case 0: obj = shoura; break;
        case 1: obj = marrin; break;
    }

    obj->transform.pos.x = x;
    obj->transform.pos.y = y;

    return obj;
}

static char *get_spells(unsigned char *data, int offset) {

}

bool load() {
    unsigned char data[SAVE_SZ];

    struct stat s;
    if (stat(savedir, &s) == -1)
        mkdir(savedir, 0700);

    if (access(savepth, F_OK) == 0) {
        FILE *saver = fopen(savepth, "rb");
        fread(data, 1, SAVE_SZ, saver);
        fclose(saver);
    } else {
        list_add_elem(party, create_member(0, 20, 20, shoura, 0, (char*)malloc(0)));
        set_room(0, 0, 0);

        return false;
    }

    set_room(data[0], 0, 0);
    
    seen_intro = data[1];
    seen_mother_greet = data[2];
    seen_marrin_greet = data[3];
    seen_ceremony = data[4];

    for (int i = 0; i < 12; i++) {
        inventory[i] = data[65 + i];
    }

    int memc = data[77];
    for (int i = 0; i < memc; i++) {
        const int offset = i*18;

        list_add_elem(party, create_member(
            data[78+offset],
            data[79+offset],
            data[80+offset],
            get_char_offset(
                data[78+offset],
                getint(data, 81+offset),
                getint(data, 85+offset)
            ),
            data[90+offset],
            get_spells(data, 91+offset)
        ));
    }    

    return true;
}