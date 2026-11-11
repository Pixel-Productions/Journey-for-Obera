#include "../falltree/engine.h"
#include "constants.h"
#include "character.h"
#include "assets.h"

#pragma once

void show_flavor(char *speech) {
    in_dialogue = true;
    can_move = false;

    txt->transform.pos.x = 28;

    txtbox->visible = true;
    txt->visible = true;

    memset(dialogue, 0, 88);

    for (int i = 0; i < strlen(speech); i++) {
        dialogue[i] = speech[i];
        usleep(33333);
    }

    while (!jfo->keyboard['Z']) {
        usleep(16667);
    }

    txtbox->visible = false;
    txt->visible = false;

    can_move = true;
}

void show_dialogue(char *speech) {
    in_dialogue = true;
    can_move = false;

    txt->transform.pos.x = 72;

    txtbox->visible = true;
    speaker->visible = true;
    txt->visible = true;

    memset(dialogue, 0, 88);

    for (int i = 0; i < strlen(speech); i++) {
        dialogue[i] = speech[i];
        usleep(33333);
    }

    while (!jfo->keyboard['Z']) {
        usleep(16667);
    }

    txtbox->visible = false;
    speaker->visible = false;
    txt->visible = false;

    can_move = true;
}


static void show_intro_flavor(object obj, char *speech) {
    memset(obj->type.text->string, 0, 88);

    for (int i = 0; i < strlen(speech); i++) {
        obj->type.text->string[i] = speech[i];
        usleep(33333);
    }
}

void intro() {
    overlay->type.rect->color = 0xFF;

    char csnmsg[110] = "";
    object csntxt = object_init(TEXT, fnt_default_white, csnmsg, 22);
    csntxt->transform.pos.x = 52;
    csntxt->transform.pos.y = 60;
    csntxt->zidx = 2;
    scene_add_object(mainscn, csntxt);

    show_intro_flavor(csntxt, "In the land of Obera,\nthere are three\nkingdoms...");
    sleep(3);
    show_intro_flavor(csntxt, "Moridia, the Magic\nPeople, Avia, the\nWinged People, and\nHume, the Mundane\nPeople.");
    sleep(3);
    show_intro_flavor(csntxt, "For 1000 years,\npeace has been kept\nbetween the\nnations...");
    sleep(3);
    show_intro_flavor(csntxt, "But the new Human\nLord Volnahm has\nbroken that peace.");
    sleep(3);
    show_intro_flavor(csntxt, "Desiring power only\nfor himself, he\nsends his forces to\ninvade the\nneighboring nations.");
    sleep(3);
    show_intro_flavor(csntxt, "Your Moridian\nvillage has not been\nattacked, but it is\nonly a matter of\ntime...");
    sleep(3);
    
    scene_remove_object(mainscn, csntxt);
    object_queue_free(jfo, csntxt);

    shoura->transform.pos.x = 156;
    shoura->transform.pos.y = 56;

    show_dialogue("Shoura! Wake up!");
    can_move = false;

    for (int i = 255; i > 0; i--) {
        overlay->type.rect->color = i;
        usleep(33333);
    }

    sleep(1);
    show_dialogue("Quickly! The ceremony has almost begun!");

    animated_sprite_set_frame(shoura, 4);
    for (int i = 0; i < 24; i++) {
        shoura->transform.pos.x -= 1;
        usleep(33333);
    }
    animated_sprite_set_frame(shoura, 0);

    can_move = true;
    seen_intro = true;
}

void mother_greet(void *obj) {
    object mother = (object)obj;

    mother->vel.y = 1;
    move_steps(mother, 16);

    mother->vel.y = 0;
    mother->vel.x = 1;
    move_steps(mother, 32);

    sleep(1);
    show_dialogue("There you are! Hurry on outside, you're just in time!");

    mother->vel.y = -1;
    mother->vel.x = 0;
    move_steps(mother, 32);

    animated_sprite_set_frame(mother, 0);
    seen_mother_greet = true;
}

void marrin_greet(void *null) {
    animated_sprite_set_frame(shoura, 8);

    marrin->vel.y = NORMALIZE(shoura->transform.pos.y - marrin->transform.pos.y);
    move_steps(marrin, abs((shoura->transform.pos.y - 8) - marrin->transform.pos.y));

    marrin->vel.y = 0;
    marrin->vel.x = NORMALIZE(shoura->transform.pos.x - marrin->transform.pos.x);
    move_steps(marrin, abs(shoura->transform.pos.x - marrin->transform.pos.x));

    animated_sprite_set_frame(marrin, 0);

    sleep(1);
    show_dialogue("Oh, Shoura.");
    show_dialogue("Marrin! Are you excited for the ceremony?");
    show_dialogue("The ceremony? Ah, yes, I suppose.");
    show_dialogue("Lead the way, then.");

    marrin->vel.y = 0;
    marrin->vel.x = -1;
    move_steps(marrin, 16);
    marrin->vel.x = 0;
    marrin->vel.y = 1;
    move_steps(marrin, 32);
    marrin->vel.y = 0;
    marrin->vel.x = 1;
    move_steps(marrin, 16);

    animated_sprite_set_frame(marrin, 8);
    marrin_in_party = true;
    seen_marrin_greet = true;
}