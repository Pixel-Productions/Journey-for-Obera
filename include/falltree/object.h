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

static int _uidc = 1;

object object_init(enum OBJECT_TYPE type, ...) {
    object obj = (object)malloc(sizeof(*obj));

    obj->uid = _uidc;
    _uidc++;

    obj->flags = list_init();
    obj->children = list_init();

    obj->lights = list_init();

    obj->type_name = type;

    obj->transform = (transform2) {
        .pos = {
            .x = 0,
            .y = 0
        },
        .scale = {
            .x = 1,
            .y = 1
        },
        .offset = {
            .x = 0,
            .y = 0
        },
        .rot = 0.0f
    };
    obj->vel = (vec2) { 0, 0 };

    obj->zidx = 0;
    obj->visible = true;

    obj->tick = NULL;

    va_list args;
    va_start(args, type);
    
    switch (type) {
        case RECT: {
            rect child = (rect)malloc(sizeof(*child));

            child->width = va_arg(args, int);
            child->height = va_arg(args, int);
            child->color = va_arg(args, color_t);

            obj->is_collider = false;
            obj->can_collide = false;

            obj->type.rect = child;

            break;
        }
        case SPRITE: {
            sprite child = (sprite)malloc(sizeof(*child));
            child->img = va_arg(args, image);

            obj->is_collider = false;
            obj->can_collide = false;

            obj->type.sprite = child;

            break;
        }
        case ANIMATED_SPRITE: {
            animated_sprite child = (animated_sprite)malloc(sizeof(*child));
            child->img = va_arg(args, image);

            child->w = va_arg(args, int);
            child->h = va_arg(args, int);
            child->hframes = va_arg(args, int);
            child->vframes = va_arg(args, int);
            child->ox = va_arg(args, int);
            child->oy = va_arg(args, int);
            child->sx = va_arg(args, int);
            child->sy = va_arg(args, int);

            child->frame = 0;

            obj->is_collider = false;
            obj->can_collide = false;

            obj->type.animated_sprite = child;

            break;
        }
        case TILE_GRID: {
            tile_grid child = (tile_grid)malloc(sizeof(*child));

            tileset set = va_arg(args, tileset);
            tilemap map = va_arg(args, tilemap);

            obj->shp = shape_init(map->width*set->tw, map->height*set->th);

            for (int x = 0; x < map->width; x++) {
                for (int y = 0; y < map->height; y++) {
                    if (set->tiles[map->data[IDX(x, y, map->width)]-1].hascoll) {
                        for (int j = 0; j < set->tw; j++) {
                            for (int k = 0; k < set->th; k++) {
                                obj->shp->data[IDX(x*set->tw+j, y*set->th+k, map->width*set->tw)] = true;
                            }
                        }
                    }
                }
            }

            child->map = map;
            child->set = set;

            obj->is_collider = true;
            obj->can_collide = true;

            obj->type.tile_grid = child;

            break;
        }
        case TEXT: {
            text child = (text)malloc(sizeof(*child));

            child->fnt = va_arg(args, font);
            child->string = va_arg(args, char*);
            child->width = va_arg(args, int);

            obj->is_collider = false;
            obj->can_collide = false;

            obj->type.text = child;

            break;
        }
        case AREA: {
            area child = (area)malloc(sizeof(*child));

            child->width = va_arg(args, int);
            child->height = va_arg(args, int);

            child->colliding = false;

            obj->is_collider = false;
            obj->can_collide = false;

            obj->type.area = child;

            break;
        }
        case ORIGIN: {
            obj->transform.pos.x = va_arg(args, int);
            obj->transform.pos.y = va_arg(args, int);

            break;
        }/*
        case VIDEO_PLAYBACK: {
            video_playback child = (video_playback)malloc(sizeof(*child));

            child->vid = load_video()
        }*/
    }

    va_end(args);

    return obj;
}

void free_object(object obj) {
    for (int i = 0; i < obj->flags->len; i++) {
        object_flag flag = (object_flag)list_get_elem(obj->flags, i);

        if (flag->type == DYNAMIC) {
            free(flag->val);
        }

        free(flag);
    }

    free_list(obj->flags);

    for (int i = 0; i < obj->children->len; i++) {
        free_object((object)list_get_elem(obj->children, i));
    }

    free_list(obj->children);

    switch (obj->type_name) {
        case RECT: {
            free(obj->type.rect);
            break;
        }
        case SPRITE: {
            free(obj->type.sprite);
            break;
        }
        case ANIMATED_SPRITE: {
            free(obj->type.animated_sprite);
            break;
        }
        case TILE_GRID: {
            free(obj->shp->data);
            free(obj->shp);
            free(obj->type.tile_grid);
            break;
        }
        case TEXT: {
            free(obj->type.text);
            break;
        }
        case AREA: {
            free(obj->type.area);
            break;
        }
    }

    free(obj);
}

// Get a flag of object `obj`.
void *object_get_flag(object obj, int idx) {
    return ((object_flag)list_get_elem(obj->flags, idx))->val;
}

int object_add_flag(object obj, void *flag, enum OBJECT_FLAG_TYPE type) {
    object_flag oflg = (object_flag)malloc(sizeof(*oflg));

    oflg->val = flag;
    oflg->type = type;

    list_add_elem(obj->flags, oflg);
    return obj->flags->len-1;
}

// Set a flag of object `obj`.
void object_set_flag(object obj, int idx, void *flag, enum OBJECT_FLAG_TYPE type) {
    object_flag oflg = (object_flag)list_get_elem(obj->flags, idx);

    oflg->val = flag;
    oflg->type = type;
}

// Add a flag to object `obj`.
void object_remove_flag(object obj, int flag) {
    object_flag oflg = (object_flag)list_get_elem(obj->flags, flag);

    if (oflg->type == DYNAMIC) {
        free(oflg->val);
    }

    free(oflg);

    list_remove_elem(obj->flags, flag);
}

// Add a child to object `obj`.
void object_add_child(object parent, object child) {
    list_add_elem(parent->children, child);

    int min_idx = 0;
 	for (int i = 0; i < parent->children->len; i++) {
		min_idx = i;

	  	for (int j = i+1; j < parent->children->len; j++) {
			if (((object)list_get_elem(parent->children, min_idx))->zidx > ((object)list_get_elem(parent->children, j))->zidx) {
				min_idx = j;
			}
  		}

		object o0 = (object)list_get_elem(parent->children, i);
        object o1 = (object)list_get_elem(parent->children, min_idx);
        list_set_elem(parent->children, i, o1);
        list_set_elem(parent->children, min_idx, o0);
 	}
}

// Remove a child from object `obj`.
void object_remove_child(object parent, object child) {
    for (int i = 0; i < parent->children->len; i++) {
        if (((object)list_get_elem(parent->children, i)) == child)
            list_remove_elem(parent->children, i);
    }
}

// Add a light source to object `obj`.
void object_add_light(object obj, light lgh) {
    list_add_elem(obj->lights, lgh);
}

// Remove a light source from object `obj`.
void object_remove_light(object obj, light lgh) {
    for (int i = 0; i < obj->lights->len; i++)
        if (list_get_elem(obj->lights, i) == lgh)
            list_remove_elem(obj->lights, i);
}

#pragma region OBJECT SPECIFIC FUNCTIONS

void object_add_collision_shape(object obj, shape shp) {
    obj->shp = shp;
    obj->is_collider = true;
    obj->can_collide = true;
}

void sprite_set_image(object obj, image img) {
    obj->type.sprite->img = img;
}

int animated_sprite_get_frame(object obj) {
    return obj->type.animated_sprite->frame;
}

void animated_sprite_set_frame(object obj, int frame) {
   obj->type.animated_sprite->frame = frame;
}

void animated_sprite_increment_frame(object obj, int amount) {
    obj->type.animated_sprite->frame += amount;
}

void animated_sprite_decrement_frame(object obj, int amount) {
    obj->type.animated_sprite->frame -= amount;
}

void tile_grid_set_tileset(object obj, tileset set) {
    obj->type.tile_grid->set = set;
}

void tile_grid_set_tilemap(object obj, tilemap map) {
    obj->type.tile_grid->map = map;
}

void tile_grid_set_tile(object obj, int x, int y, int tile) {
    tilemap map = obj->type.tile_grid->map;
    map->data[IDX(x, y, map->width)] = tile;
}

char *text_get_string(object obj) {
    return obj->type.text->string;
}

void text_set_string(object obj, char *string) {
    obj->type.text->string = string;
}

bool area_is_colliding(object obj) {
    return obj->type.area->colliding;
}

#pragma endregion

#ifdef __cplusplus
}
#endif