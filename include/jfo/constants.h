#include "../falltree/engine.h"

#ifndef JFO_CONSTANTS_H
#define JFO_CONSTANTS_H

game jfo;

unsigned long t = 0;

scene titlescn;
scene mainscn;

object origin;


object title_menu_bg;

object shoura;
object marrin;


object member_select;

object inventory_bg;

object inventory_info_bg;
object inventory_info;
object inventory_slots[12];

object spells_bg;

int current_room = -1;

bool seen_intro = false;
bool seen_mother_greet = false;
bool seen_marrin_greet = false;

bool marrin_in_party = false;

bool seen_ceremony = false;


object overlay;


bool in_dialogue = false;
char dialogue[88] = "";
object txtbox;
object speaker;
object txt;


int max_speed = 1;
bool can_move = true;


typedef struct {
    unsigned char id;

    object obj;
    
    unsigned char hp;
    unsigned char pw;

    unsigned char spellc;
    char spells[6];
} *member;

int itemc = 0;
char inventory[12] = { 0 };

list party;

#endif