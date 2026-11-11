#include "include/falltree/engine.h"

game tileview;
scene mainscn;

void tick(game gm, enum EVENT_PHASE phase) {}

int main(int argc, char **argv) {
    tileset set = load_tileset(argv[1]);
    tilemap map = load_tilemap(argv[2]);

    tileview = game_init("Tilemap Viewer", map->width*set->tw, map->height*set->th, 2);

    mainscn = scene_init();

    object tg = object_init(TILE_GRID, set, map);
    scene_add_object(mainscn, tg);

    game_add_scene(tileview, mainscn);

    tileview->tick = tick;
    game_start(tileview);

    free_tileset(set);
    free_tilemap(map);

    return 0;
}