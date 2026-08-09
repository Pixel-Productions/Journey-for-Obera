#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include "physics.h"
#include "types.h"
#include "list.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

object object_init(enum OBJECT_TYPE type, ...) {
    object obj = (object)malloc(sizeof(*obj));

    obj->type = type;

    obj->pos = vec2_init(0, 0);
    obj->vel = vec2_init(0, 0);

    obj->data = list_init();

    obj->tick = NULL;

    va_list args;
    va_start(args, type);
    
    switch (type) {
        case CUSTOM: {
            list_add_elem(obj->data, (void*)va_arg(args, object_destructor_func));

            obj->is_collider = false;
            obj->can_collide = false;

            break;
        }
        case SPRITE: {
            list_add_elem(obj->data, (image)(va_arg(args, asset)->val));

            obj->is_collider = false;
            obj->can_collide = false;

            break;
        }
        case ANIMATED_SPRITE: {
            list_add_elem(obj->data, (image)(va_arg(args, asset)->val));

            int *w = (int*)malloc(sizeof(int));
            int *h = (int*)malloc(sizeof(int));
            int *hframes = (int*)malloc(sizeof(int));
            int *vframes = (int*)malloc(sizeof(int));
            int *sx = (int*)malloc(sizeof(int));
            int *sy = (int*)malloc(sizeof(int));
            int *ox = (int*)malloc(sizeof(int));
            int *oy = (int*)malloc(sizeof(int));
            int *frame = (int*)malloc(sizeof(int));

            *w = va_arg(args, int);
            *h = va_arg(args, int);
            *hframes = va_arg(args, int);
            *vframes = va_arg(args, int);
            *sx = va_arg(args, int);
            *sy = va_arg(args, int);
            *ox = va_arg(args, int);
            *oy = va_arg(args, int);
            *frame = 0;

            list_add_elem(obj->data, w);
            list_add_elem(obj->data, h);
            list_add_elem(obj->data, hframes);
            list_add_elem(obj->data, vframes);
            list_add_elem(obj->data, sx);
            list_add_elem(obj->data, sx);
            list_add_elem(obj->data, ox);
            list_add_elem(obj->data, oy);
            list_add_elem(obj->data, frame);

            obj->is_collider = false;
            obj->can_collide = false;

            break;
        }
        case TILE_GRID: {
            tileset set = (tileset)va_arg(args, asset)->val;
            tilemap map = (tilemap)va_arg(args, asset)->val;

            obj->shp = shape_init(map->width*set->tw, map->height*set->th);

            for (int i = 0; i < map->width*map->height; i++) {
                if (set->tiles[map->data[i]].hascoll) {
                    shape shp = shape_init(set->tw, set->th);
                    memset(shp->data, true, shp->width*shp->height);
                    blend_shapes(obj->shp, shp);
                }
            }

            list_add_elem(obj->data, set);
            list_add_elem(obj->data, map);

            obj->is_collider = true;
            obj->can_collide = true;

            break;
        }
    }

    va_end(args);

    return obj;
}

#pragma region OBJECT SPECIFIC FUNCTIONS

void sprite_set_image(object obj, asset ast) {
    list_set_elem(obj->data, 0, ast->val);
}

int animated_sprite_get_frame(object obj) {
    return VOID_PTR_CAST(list_get_elem(obj->data, 9)->val, int);
}

void animated_sprite_set_frame(object obj, int frame) {
    list_elem elem = list_get_elem(obj->data, 9);
    VOID_PTR_CAST(elem->val, int) = frame;
}

void animated_sprite_increment_frame(object obj, int amount) {
    animated_sprite_set_frame(obj, VOID_PTR_CAST(list_get_elem(obj->data, 9)->val, int)+amount);
}

void animated_sprite_decrement_frame(object obj, int amount) {
    animated_sprite_set_frame(obj, VOID_PTR_CAST(list_get_elem(obj->data, 9)->val, int)-amount);
}

#pragma endregion

#ifdef __cplusplus
}
#endif