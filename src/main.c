#include "../include/falltree/engine.h"
#include "../include/falltree/object.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/jfo/constants.h"
#include "../include/jfo/room.h"
#include "../include/jfo/assets.h"
#include "../include/jfo/character.h"
#include "../include/jfo/events.h"
#include "../include/jfo/save.h"
#include "../include/jfo/item.h"

const char *credits = "Journey for Obera\n\
\n\
-- Art --\n\
Main Artist ------------- p1xel12\n\
Cutscene Artist --------- Beckett Feussner\n\
\n\
-- Development --\n\
Lead Developer ---------- p1xel12";


void button_tick(game gm, object obj) {
    if (gm->mouse.x >= obj->transform.pos.x && gm->mouse.x <= obj->transform.pos.x + 8 * strlen(obj->type.text->string) && gm->mouse.y >= obj->transform.pos.y && gm->mouse.y <= obj->transform.pos.y + 8) {
        if (gm->mouse.button == 0) {
            switch (*(int*)object_get_flag(obj, 0)) {
                case 0: {
                    jfo->active_scene = 1;
                    t = 0;

                    break;
                }
            }
        }
    }
}


void shoura_tick(game gm, object obj) {
    if (can_move) {
        if (gm->keyboard['D'])
            obj->vel.x = CLAMP(obj->vel.x+1, -max_speed, max_speed);
        else if (gm->keyboard['A'])
            obj->vel.x = CLAMP(obj->vel.x-1, -max_speed, max_speed);
        else if (obj->vel.x != 0)
            obj->vel.x -= obj->vel.x > 0 ? 1 : -1;

        if (gm->keyboard['S'])
            obj->vel.y = CLAMP(obj->vel.y+1, -max_speed, max_speed);
        else if (gm->keyboard['W'])
            obj->vel.y = CLAMP(obj->vel.y-1, -max_speed, max_speed);
        else if (obj->vel.y != 0)
            obj->vel.y -= obj->vel.y > 0 ? 1 : -1;
    }

    move_character(obj);
    update_camera_pos();
}

void marrin_tick(game gm, object obj) {
    if (marrin_in_party) {
        if (shoura->vel.x != 0)
            obj->vel.x = NORMALIZE((shoura->transform.pos.x - shoura->vel.x*24) - obj->transform.pos.x);
        else
            obj->vel.x = 0;

        if (shoura->vel.y != 0)
            obj->vel.y = NORMALIZE((shoura->transform.pos.y - shoura->vel.y*24) - obj->transform.pos.y);
        else
            obj->vel.y = 0;

        move_character(obj);
    }
}


void tick(game gm, enum EVENT_PHASE phase) {
    switch (phase) {
        case PRE: {
            t++;

            if (t == 1) {
                switch (gm->active_scene) {
                    case 0: {
                        sleep(1);

                        for (int i = 0; i < 5; i++) {
                            animated_sprite_set_frame(title_menu_bg, i);

                            switch (i) {
                                case 2: {
                                    object btn = object_init(TEXT, fnt_small, "Continue");

                                    btn->transform.pos.x = 22;
                                    btn->transform.pos.y = 85;
                                    btn->tick = button_tick;
                                    btn->zidx = 1;

                                    int *id = (int*)malloc(sizeof(int));
                                    *id = 0;
                                    object_add_flag(btn, id, DYNAMIC);

                                    scene_add_object(titlescn, btn);
                                    break;
                                }
                                case 3: {
                                    object btn = object_init(TEXT, fnt_small, "New Game");
                                    
                                    btn->transform.pos.x = 22;
                                    btn->transform.pos.y = 101;
                                    btn->tick = button_tick;
                                    btn->zidx = 1;

                                    int *id = (int*)malloc(sizeof(int));
                                    *id = 1;
                                    object_add_flag(btn, id, DYNAMIC);

                                    scene_add_object(titlescn, btn);
                                    break;
                                }
                                case 4: {
                                    object btn = object_init(TEXT, fnt_small, "Credits");
                                    
                                    btn->transform.pos.x = 22;
                                    btn->transform.pos.y = 117;
                                    btn->tick = button_tick;
                                    btn->zidx = 1;

                                    int *id = (int*)malloc(sizeof(int));
                                    *id = 2;
                                    object_add_flag(btn, id, DYNAMIC);

                                    scene_add_object(titlescn, btn);
                                    break;
                                }
                            }

                            usleep(100000);
                        }

                        break;
                    }
                    case 1: {
                        if (!seen_intro) intro();

                        break;
                    }
                }
            }

            if (gm->keyboard[KEY_ESCAPE]);

            if (gm->keyboard['Q']) {
                inventory_bg->visible = !inventory_bg->visible;

                if (inventory_bg->visible) {
                    for (int i = 0; i < 12; i++) {
                        sprite_set_image((object)object_get_flag(inventory_bg, i), itemreg[inventory[i]].img);
                    }
                }
            }

            break;
        }
        case POST: {

            break;
        }
    }
}


int main() {
    jfo = game_init("Journey for Obera", 280, 160, 4);

    party = list_init();

    init_savesys();
    load_assets();

    titlescn = scene_init();
    mainscn = scene_init();

    origin = object_init(ORIGIN, 0, 0);
    scene_add_object(mainscn, origin);

    overlay = object_init(RECT, 280, 160, 0);
    overlay->zidx = 2;
    scene_add_object(mainscn, overlay);
    scene_add_object(titlescn, overlay);


    object title = object_init(SPRITE, img_title);
    scene_add_object(titlescn, title);

    title_menu_bg = object_init(ANIMATED_SPRITE, img_title_menu, 96, 80, 6, 1, 0, 0, 0, 0);
    title_menu_bg->transform.pos.x = 16;
    title_menu_bg->transform.pos.y = 64;
    scene_add_object(titlescn, title_menu_bg);


    shoura = object_init(ANIMATED_SPRITE, img_shoura, 16, 16, 4, 4, 0, 0, 0, 0);
    shoura->transform.pos.x = 132;
    shoura->transform.pos.y = 72;
    shoura->tick = shoura_tick;
    shoura->zidx = 2;
    object_add_collision_shape(shoura, shp_half_slim);
    object_add_child(origin, shoura);

    marrin = object_init(ANIMATED_SPRITE, img_marrin, 16, 16, 4, 4, 0, 0, 0, 0);
    marrin->transform.pos.x = 132;
    marrin->transform.pos.y = 8;
    marrin->zidx = 1;
    marrin->tick = marrin_tick;
    object_add_collision_shape(marrin, shp_full);


    rooms[SHOURA_ROOM] = room_init(ts_house, tm_shoura_room);
    rooms[SHOURA_HALLWAY] = room_init(ts_house, tm_shoura_hallway);
    rooms[SHOURA_KITCHEN] = room_init(ts_house, tm_shoura_kitchen);
    rooms[VILLAGE] = room_init(ts_overworld, tm_village);
    rooms[CEREMONY] = room_init(ts_overworld, tm_ceremony);

    room_add_warps(SHOURA_ROOM, 1,   3, 8, 1, 1, SHOURA_HALLWAY, 8, 3);
    room_add_warps(SHOURA_HALLWAY, 2,   8, 2, 1, 1, SHOURA_ROOM, 3, 7,   0, 3, 1, 1, SHOURA_KITCHEN, 7, 5);
    room_add_warps(SHOURA_KITCHEN, 2,   8, 5, 1, 1, SHOURA_HALLWAY, 1, 3,   4, 8, 1, 1, VILLAGE, 5, 16);
    room_add_warps(VILLAGE, 2,   5, 15, 1, 1, SHOURA_KITCHEN, 4, 7,   11, 0, 1, 1, CEREMONY, 9, 15);
    room_add_warps(CEREMONY, 1,   9, 16, 1, 1, VILLAGE, 11, 1);


    object blanket = object_init(SPRITE, img_shoura_blanket);
    blanket->transform.pos.x = 64;
    blanket->transform.pos.y = 49;
    blanket->zidx = 3;
    room_add_object(SHOURA_ROOM, blanket);

    object mother = object_init(ANIMATED_SPRITE, img_mother, 16, 16, 4, 4, 0, 0, 0, 0);
    mother->zidx = 1;
    object_add_collision_shape(mother, shp_full);
    room_add_object(SHOURA_KITCHEN, mother);
    
    room_add_object(VILLAGE, marrin);

    object pedestal = object_init(SPRITE, img_pedestal);
    pedestal->transform.pos.x = 152;
    pedestal->transform.pos.y = 108;
    pedestal->zidx = 1;
    object_add_collision_shape(pedestal, shp_full);
    room_add_object(CEREMONY, pedestal);


    txtbox = object_init(SPRITE, img_dialogue_bg);
    txtbox->transform.pos.x = 12;
    txtbox->transform.pos.y = 92;
    txtbox->zidx = 3;
    txtbox->visible = false;
    scene_add_object(mainscn, txtbox);

    speaker = object_init(SPRITE, img_portrait_mother);
    speaker->transform.pos.x = 28;
    speaker->transform.pos.y = 108;
    speaker->zidx = 4;
    speaker->visible = false;
    scene_add_object(mainscn, speaker);

    txt = object_init(TEXT, fnt_default, dialogue, 22);
    txt->transform.pos.x = 72;
    txt->transform.pos.y = 108;
    txt->zidx = 4;
    txt->visible = false;
    scene_add_object(mainscn, txt);


    game_add_scene(jfo, titlescn);
    game_add_scene(jfo, mainscn);

    jfo->active_scene = !load();

    if (!seen_mother_greet)  {
        create_cutscene_trigger(SHOURA_KITCHEN, 80, 80, 16, 16, mother_greet, mother);

        mother->transform.pos.x = 48;
        mother->transform.pos.y = 80;
    } else {
        mother->transform.pos.x = 96;
        mother->transform.pos.y = 48;
    }

    if (!seen_marrin_greet) create_cutscene_trigger(VILLAGE, 144, 160, 48, 16, marrin_greet, NULL);


    jfo->tick = tick;
    game_start(jfo);

    for (int i = 0; i < ROOMS; i++) {
        room rm = rooms[i];
        free_object(rm);

        free(rm);
    }
    
    return 0;
}