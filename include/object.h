#include <stdlib.h>
#include <stdbool.h>
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

    obj->can_collide = true;
    obj->is_collider = true;

    obj->pos = vec2_init(0, 0);
    obj->vel = vec2_init(0, 0);

    obj->data = list_init();

    va_list args;
    
    switch (type) {
        case CUSTOM: {
            va_start(args, type);
            object_destructor_func destr = va_arg(args, object_destructor_func);
            list_add_elem(obj->data, (void*)destr);
        }
        case SPRITE: {
            va_start(args, type);
            image img = VOID_PTR_CAST(va_arg(args, asset).val, image);
            list_add_elem(obj->data, (void*)&img);
        }
        case TILE_GRID: {
            va_start(args, type);
            tileset set = VOID_PTR_CAST(va_arg(args, asset).val, tileset);
            tilemap map = VOID_PTR_CAST(va_arg(args, asset).val, tilemap);

            obj->shp = shape_init(map.width*set.tw, map.height*set.th);

            for (int i = 0; i < map.width*map.height; i++) {
                if (set.tiles[map.data[i]].hascoll) {
                    shape shp = shape_init(set.tw, set.th);
                    memset(shp.data, true, shp.width*shp.height);
                    blend_shapes(&obj->shp, shp);
                }
            }  // Pan flag! Yayy! :D 
        } // 5 seconds after typing this i realized this makes no fcking sense if you don't view it in vscode on the "Dark modern" color theme
    } // Bottom text

    va_end(args);

    return obj;
}

#ifdef __cplusplus
}
#endif