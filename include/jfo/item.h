#include "constants.h"
#include "assets.h"

#ifndef JFO_ITEM_H
#define JFO_ITEM_H

enum ITEM_TYPE {
    CONSUMABLE = 0,
    MATERIAL = 1,
    USABLE = 2
};

typedef struct item item;

typedef struct {
    char health;
} consumable_data;

typedef void (*item_use_func)(item);
typedef struct {
    item_use_func use;
} usable_data;

typedef union {
    consumable_data consumable_data;
    usable_data usable_data;
} item_data;

struct item {
    int id;

    enum ITEM_TYPE type;

    char *name;
    char *desc;
    image img;

    item_data data;
};

item itemreg[] = {
    (item) {
        .id = 0,
        
        .type = CONSUMABLE,
        .name = "Apple",
        .desc = "A sweet, juicy fruit anyone can enjoy.",
        .img = img_apple,

        .data = (consumable_data) {
            .health = 3,
        }
    }
};


static char *get_type_name(enum ITEM_TYPE type) {
    switch (type) {
        case CONSUMABLE: return "Consumable";
        case MATERIAL: return "Material";
        case USABLE: return "Usable";
    }
}

static char *get_data_string(item itm) {
    char data[16];
    
    switch (itm.type) {
        case CONSUMABLE: {
            sprintf(data, ": HL %d", itm.data.consumable_data.health);
            break;
        }
        default: return "";
    }

    return data;
}

static member choose_member() {
    member_select->visible = true;

    member mem;
    while (mem == NULL) {
        for (int i = 0; i < party->len; i++) {
            object slot = (object)object_get_flag(member_select, i);

            if (
                jfo->mouse.x >= slot->transform.pos.x &&
                jfo->mouse.x <= slot->transform.pos.x+16 &&
                jfo->mouse.y >= slot->transform.pos.y &&
                jfo->mouse.y <= slot->transform.pos.y+16 &&
                jfo->mouse.button == 0
            ) {
                mem = (member)list_get_elem(party, i);
            }
        }
    }

    member_select->visible = false;

    return mem;
}

static void slot_tick(game gm, object obj) {
    if (gm->mouse.x >= obj->transform.pos.x && gm->mouse.x <= obj->transform.pos.x+16 && gm->mouse.y >= obj->transform.pos.y & gm->mouse.y <= obj->transform.pos.y+16) {
        int idx = *(int*)object_get_flag(obj, 0);
        char id = inventory[idx];

        if (id != -1) {
            item itm = itemreg[id];

            char info[64];
            sprintf(info, "%s - %s%s\n%s", itm.name, get_type_name(itm.type), get_data_string(itm), itm.desc);

            text_set_string(inventory_info, info);
            inventory_info_bg->visible = true;

            if (gm->mouse.button == 0) {
                switch (itm.type) {
                    case CONSUMABLE: {
                        choose_member()->hp += itm.data.consumable_data.health;
                        inventory[idx] = 0;

                        break;
                    }
                    case USABLE: {
                        itm.data.usable_data.use(itm);
                        break;
                    }
                }
            }
        }
    } else
        inventory_info_bg->visible = false;
}

void init_inventory() {
    member_select = object_init(SPRITE, img_member_select);
    member_select->visible = false;
    
    object select_text = object_init(TEXT, fnt_default, "Select");
    object_add_child(member_select, select_text);

    for (int i = 0; i < 3; i++) {
        object mem_slot = object_init(SPRITE, NULL);

        object_add_flag(member_select, mem_slot, STATIC);
        object_add_child(member_select, mem_slot);
    }

    inventory_bg = object_init(SPRITE, img_inventory_bg);

    inventory_bg->transform.pos.x = 100;
    inventory_bg->transform.pos.y = 48;
    inventory_bg->zidx = 1;
    inventory_bg->visible = false;

    scene_add_object(mainscn, inventory_bg);

    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 6; y++) {
            object slot = object_init(SPRITE, NULL);

            int *idx = (int*)malloc(sizeof(int));
            *idx = IDX(x, y, 2);

            object_add_flag(slot, idx, DYNAMIC);
            slot->tick = slot_tick;
        }
    }
}

#endif