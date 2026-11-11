#include "../falltree/engine.h"
#include "constants.h"

#pragma once

typedef object room;

#define SHOURA_ROOM 0
#define SHOURA_HALLWAY 1
#define SHOURA_KITCHEN 2
#define VILLAGE 3
#define CEREMONY 4

#define ROOMS 5
room rooms[ROOMS];

void update_camera_pos() {
    tile_grid tg = rooms[current_room]->type.tile_grid;

    int w = tg->set->tw*tg->map->width;
    int h = tg->set->th*tg->map->height;
    
    if (w <= 280) {
        origin->transform.pos.x = 0;
    } else {
        origin->transform.pos.x = CLAMP(132 - shoura->transform.pos.x, 140 - w/2, w/2 - 140);
    }

    if (h <= 160) {
        origin->transform.pos.y = 0;
    } else {
        origin->transform.pos.y = CLAMP(72 - shoura->transform.pos.y, 80 - h/2, h/2 - 80);
    }
}

room room_init(tileset set, tilemap map) {
    room rm = (room)malloc(sizeof(*rm));

    rm = object_init(TILE_GRID, set, map);
    rm->transform.pos.x = 140 - map->width*set->tw / 2;
    rm->transform.pos.y = 80 - map->height*set->th / 2;

    return rm;
}

void set_room(int id, int px, int py) {
    room rm = rooms[id];

    can_move = false;
    for (int i = 0; i < 256; i++) {
        overlay->type.rect->color = i;
        usleep(1000);
    }

    if (current_room != -1) {
        room current = rooms[current_room];

        object_remove_child(origin, current);
    }

    current_room = id;

    object_add_child(origin, rm);

    tileset set = rm->type.tile_grid->set;
    shoura->transform.pos.x = rm->transform.pos.x + px*set->tw;
    shoura->transform.pos.y = rm->transform.pos.y + py*set->th;

    update_camera_pos();

    for (int i = 255; i >= 0; i--) {
        overlay->type.rect->color = i;
        usleep(1000);
    }
    can_move = true;
}

void warp_tick(game gm, object obj) {
    if (area_is_colliding(obj)) {
        int *data = (int*)object_get_flag(obj, 0);
        set_room(data[0], data[1], data[2]);
    }
}

void room_add_warps(int id, int warpc, ...) {
    room rm = rooms[id];

    tileset set = rm->type.tile_grid->set;

    va_list args;

    va_start(args, warpc);
    for (int i = 0; i < warpc; i++) {
        int x = va_arg(args, int)*set->tw;
        int y = va_arg(args, int)*set->th;

        object warp = object_init(AREA, va_arg(args, int)*set->tw, va_arg(args, int)*set->th);
        warp->transform.pos.x = x;
        warp->transform.pos.y = y;
        warp->zidx = 3;

        int *data = (int*)malloc(3*sizeof(int));
        data[0] = va_arg(args, int);
        data[1] = va_arg(args, int);
        data[2] = va_arg(args, int);
        object_add_flag(warp, data, DYNAMIC);

        warp->tick = warp_tick;

        object_add_child(rm, warp);
    }
}

void room_add_object(int id, object obj) {
    room rm = rooms[id];
    object_add_child(rm, obj);
}

void room_remove_object(int id, object obj) {
    room rm = rooms[id];
    object_remove_child(rm, obj);
}

void trigger_tick(game gm, object obj) {
    if (area_is_colliding(obj)) {
        ((void (*)(void *))object_get_flag(obj, 0))(object_get_flag(obj, 1));

        room rm = rooms[*(int*)object_get_flag(obj, 2)];
        object_remove_child(rm, obj);

        object_queue_free(jfo, obj);
    }
}

void create_cutscene_trigger(int id, int x, int y, int w, int h, void (*func)(void *), void *arg) {
    object trig = object_init(AREA, w, h);
    trig->transform.pos.x = x;
    trig->transform.pos.y = y;
    trig->zidx = 2;

    object_add_flag(trig, (void*)func, STATIC);
    object_add_flag(trig, arg, STATIC);

    int *rid = (int*)malloc(sizeof(int));
    *rid = id;
    object_add_flag(trig, rid, DYNAMIC);

    trig->tick = trigger_tick;

    room_add_object(id, trig);
}