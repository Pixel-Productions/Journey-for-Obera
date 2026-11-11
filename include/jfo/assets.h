#include "../falltree/engine.h"
#include "constants.h"

#ifndef JFO_ASSETS_H
#define JFO_ASSETS_H

image img_title;

image img_title_menu;

image img_shoura;
image img_mother;
image img_marrin;

shape shp_half;
shape shp_full;
shape shp_half_slim;

font fnt_default;
font fnt_default_white;
font fnt_small;

tileset ts_house;
tileset ts_overworld;

tilemap tm_shoura_room;
tilemap tm_shoura_hallway;
tilemap tm_shoura_kitchen;
tilemap tm_village;
tilemap tm_ceremony;

image img_shoura_blanket;
image img_pedestal;

image img_dialogue_bg;
image img_inventory_bg;
image img_info_bg;
image img_member_select;

image img_portrait_mother;

image img_particle_fireball;
image img_particle_heal;

image img_apple;


void load_assets() {
    img_title = register_asset(jfo, IMAGE, "assets/textures/ui/title.png");

    img_title_menu = register_asset(jfo, IMAGE, "assets/textures/ui/title_menu.png");

    img_shoura = register_asset(jfo, IMAGE, "assets/textures/characters/shoura.png");
    img_mother = register_asset(jfo, IMAGE, "assets/textures/characters/mother.png");
    img_marrin = register_asset(jfo, IMAGE, "assets/textures/characters/marrin.png");

    shp_half = register_asset(jfo, SHAPE, "assets/data/shapes/half.fsf");
    shp_full = register_asset(jfo, SHAPE, "assets/data/shapes/full.fsf");
    shp_half_slim = register_asset(jfo, SHAPE, "assets/data/shapes/half_slim.fsf");

    fnt_default = register_asset(jfo, FONT, "assets/data/fonts/font_default.ffnf");
    fnt_default_white = register_asset(jfo, FONT, "assets/data/fonts/font_default_white.ffnf");
    fnt_small = register_asset(jfo, FONT, "assets/data/fonts/font_small.ffnf");

    ts_house = register_asset(jfo, TILESET, "assets/data/tilesets/house.ftsf");
    ts_overworld = register_asset(jfo, TILESET, "assets/data/tilesets/overworld.ftsf");

    tm_shoura_room = register_asset(jfo, TILEMAP, "assets/data/tilemaps/shoura_room.ftmf");
    tm_shoura_hallway = register_asset(jfo, TILEMAP, "assets/data/tilemaps/shoura_hallway.ftmf");
    tm_shoura_kitchen = register_asset(jfo, TILEMAP, "assets/data/tilemaps/shoura_kitchen.ftmf");
    tm_village = register_asset(jfo, TILEMAP, "assets/data/tilemaps/village.ftmf");
    tm_ceremony = register_asset(jfo, TILEMAP, "assets/data/tilemaps/ceremony.ftmf");

    img_shoura_blanket = register_asset(jfo, IMAGE, "assets/textures/shoura_blanket.png");
    img_pedestal = register_asset(jfo, IMAGE, "assets/textures/pedestal.png");

    img_dialogue_bg = register_asset(jfo, IMAGE, "assets/textures/ui/dialogue_bg.png");
    img_inventory_bg = register_asset(jfo, IMAGE, "assets/textures/ui/inventory_bg.png");
    img_info_bg = register_asset(jfo, IMAGE, "assets/textures/ui/info_bg.png");
    img_member_select = register_asset(jfo, IMAGE, "assets/textures/ui/member_select.png");

    img_portrait_mother = register_asset(jfo, IMAGE, "assets/textures/portraits/mother.png");

    img_particle_fireball = register_asset(jfo, IMAGE, "assets/textures/particles/fireball.png");
    img_particle_heal = register_asset(jfo, IMAGE, "assets/textures/particles/heal.png");

    img_apple = register_asset(jfo, IMAGE, "assets/textures/items/apple.png");
}

#endif