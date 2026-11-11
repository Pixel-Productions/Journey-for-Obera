#include "include/falltree/engine.h"
#include "include/falltree/object.h"

game editor;
scene mainscn;

object tg;
int active_tile = 1;

char *name;

void move_obj(game gm, object obj) {
    if (gm->keyboard[KEY_LEFT]) obj->transform.pos.x -= 5;
    if (gm->keyboard[KEY_RIGHT]) obj->transform.pos.x += 5;

    if (gm->keyboard[KEY_UP]) obj->transform.pos.y -= 5;
    if (gm->keyboard[KEY_DOWN]) obj->transform.pos.y += 5;
}

void editor_tick(game gm, enum EVENT_PHASE phase) {
    if (phase == PRE) {
        move_obj(gm, tg);
    }   
}

void tstile_tick(game gm, object obj) {
    int *data = object_get_flag(obj, 0);
    if (gm->mouse.x >= obj->transform.pos.x && gm->mouse.x < obj->transform.pos.x+obj->type.rect->width && gm->mouse.y >= obj->transform.pos.y && gm->mouse.y < obj->transform.pos.y+obj->type.rect->height && gm->mouse.button == 0) {
        active_tile = *data;
    }

    obj->type.rect->color = *data == active_tile ? 0xFF000050 : 0x00000000;
}

void tile_tick(game gm, object obj) {
    move_obj(gm, obj);

    if (gm->mouse.x >= obj->transform.pos.x && gm->mouse.x < obj->transform.pos.x+obj->type.rect->width && gm->mouse.y >= obj->transform.pos.y && gm->mouse.y < obj->transform.pos.y+obj->type.rect->height) {
        int *data = object_get_flag(obj, 0);
        if (gm->mouse.button == 0) {
            tile_grid_set_tile(tg, data[0], data[1], active_tile);
        } else if (gm->mouse.button == 1) {
            tile_grid_set_tile(tg, data[0], data[1], 0);
        }
    }
}

void btn_tick(game gm, object obj) {
    if (gm->mouse.x >= obj->transform.pos.x && gm->mouse.x < obj->transform.pos.x+obj->type.rect->width && gm->mouse.y >= obj->transform.pos.y && gm->mouse.y < obj->transform.pos.y+obj->type.rect->height && gm->mouse.button == 0) {
        tilemap map = tg->type.tile_grid->map;

        unsigned char buf[4+map->width*map->height];
        buf[1] = map->width >> 8 & 0xFF;
        buf[0] = map->width & 0xFF;
        buf[3] = map->height >> 8 & 0xFF;
        buf[2] = map->height & 0xFF;

        for (int i = 0; i < map->width*map->height; i++) {
            buf[i+4] = (char)map->data[i];
        }

        FILE *file = fopen(name, "wb");
        fwrite(buf, 1, 4+map->width*map->height, file);

        fclose(file);

        glfwSetWindowShouldClose(gm->win, true);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Error: invalid arguments\n");
        exit(-1);
    }

    font fnt_default = load_font("assets/data/fonts/font_default.ffnf");

    int w = atoi(argv[1]);
    int h = atoi(argv[2]);

    name = argv[4];

    mainscn = scene_init();

    tilemap map = (tilemap)malloc(sizeof(*map));
    map->width = w;
    map->height = h;
    map->data = (int*)calloc(w*h, sizeof(int));

    editor = game_init("Tilemap Editor", 640, 360, 2);
    editor->bgcol = 0xFFFFFFF;

    tileset set = load_tileset(argv[3]);

    object img = object_init(SPRITE, set->map);
    scene_add_object(mainscn, img);

    for (int i = 0; i < set->tilec; i++) {
        tile tl = set->tiles[i];
        object tlo = object_init(RECT, set->tw, set->th, 0x00000000);

        tlo->transform.pos.x = tl.x;
        tlo->transform.pos.y = tl.y;

        int *data = (int*)malloc(sizeof(int));
        *data = i+1;
        object_add_flag(tlo, data, DYNAMIC);

        tlo->tick = tstile_tick;

        scene_add_object(mainscn, tlo);
    }

    tg = object_init(TILE_GRID, set, map);
    tg->zidx = 1;
    tg->transform.pos.x = 320 - set->tw*w / 2;
    tg->transform.pos.y = 180 - set->th*h / 2;
    scene_add_object(mainscn, tg);

    bool last = true;
    for (int x = 0; x < w; x++) {
        for (int y = 0; y < h; y++) {
            object tile = object_init(RECT, set->tw, set->th, last ? 0xFFFFFFFF : 0x777777FF);
            last = !last;

            tile->transform.pos.x = tg->transform.pos.x+x*set->tw;
            tile->transform.pos.y = tg->transform.pos.y+y*set->th;
            tile->tick = tile_tick;

            int *data = (int*)malloc(2*sizeof(int));
            data[0] = x;
            data[1] = y;
            object_add_flag(tile, data, DYNAMIC);

            scene_add_object(mainscn, tile);
        }
    }

    object btn = object_init(RECT, 36, 12, 0xFF0000FF);
    btn->transform.pos.x = 602;
    btn->transform.pos.y = 2;
    btn->tick = btn_tick;
    scene_add_object(mainscn, btn);

    object btntxt = object_init(TEXT, fnt_default, "Save");
    btntxt->transform.pos.x = 604;
    btntxt->transform.pos.y = 4;
    scene_add_object(mainscn, btntxt);

    game_add_scene(editor, mainscn);

    editor->tick = editor_tick;
    game_start(editor);

    free(map->data);
    free(map);

    free(set->tiles);
    free(set);

    return 0;
}