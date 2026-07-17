#include "../include/engine.h"
#include "../include/object.h"

void update(game gm) {
    
}

object tree;

int main() {
    game oneday = init_game("OneDay", 160, 90, 4);
    load_asset(oneday, "tree", "assets/textures/fall_tree.fif", IMAGE);

    tree = object_init(SPRITE, get_asset(oneday, "tree"));
    tree->pos.x = 5;
    tree->pos.y = 5;

    list_add_elem(oneday->objects, tree);

    image img = VOID_PTR_CAST(get_asset(oneday, "tree").val, image);
    printf("%d, %d\r\n", img.width, img.height);

    oneday->tick = update;

    game_start(oneday);

    end_game(oneday);
}