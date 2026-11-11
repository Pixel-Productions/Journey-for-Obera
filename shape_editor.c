#include "include/falltree/engine.h"
#include "include/falltree/object.h"

game editor;
scene mainscn;

object tg;

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

void tile_tick(game gm, object obj) {
    move_obj(gm, obj);

    if (gm->mouse.x >= obj->transform.pos.x && gm->mouse.x < obj->transform.pos.x+obj->type.rect->width && gm->mouse.y >= obj->transform.pos.y && gm->mouse.y < obj->transform.pos.y+obj->type.rect->height) {
        int *data = object_get_flag(obj, 0);
        if (gm->mouse.button == 0) {
            tile_grid_set_tile(tg, data[0], data[1], true);
        } else if (gm->mouse.button == 1) {
            tile_grid_set_tile(tg, data[0], data[1], false);
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
            buf[i+4] = (bool)map->data[i];
        }

        FILE *file = fopen(name, "wb");
        fwrite(buf, 1, 4+map->width*map->height, file);

        fclose(file);

        glfwSetWindowShouldClose(gm->win, true);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Error: invalid arguments\n");
        exit(-1);
    }

    font fnt_default = load_font("assets/data/fonts/font_default.ffnf");

    int w = atoi(argv[1]);
    int h = atoi(argv[2]);

    name = argv[3];

    mainscn = scene_init();

    editor = game_init("Shape Editor", 640, 360, 2);
    editor->bgcol = 0xFFFFFFF;

    image img = (image)malloc(sizeof(*img));
    img->width = 16;
    img->height = 16;
    img->data = (color_t*)malloc(256*sizeof(color_t));
    memset(img->data, 0x0040FFA0, 256*sizeof(color_t));

    tileset set = (tileset)malloc(sizeof(*set));
    set->map = img;
    set->tw = 16;
    set->th = 16;
    set->tox = 0;
    set->toy = 0;
    set->tsx = 0;
    set->tsy = 0;

    set->tiles = (tile*)malloc(w*h*sizeof(tile));
    *set->tiles = (tile) { .x = 0, .y = 0, .hascoll = 0 };

    set->tilec = 1;

    tilemap map = (tilemap)malloc(sizeof(*map));
    map->width = w;
    map->height = h;
    map->data = (int*)calloc(sizeof(int), w*h);

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