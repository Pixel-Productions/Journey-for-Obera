// List implementation by FireDude123 as part of the FallTree game engine

#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

list list_init() {
    list ls = (list)malloc(sizeof(*ls));
    ls->len = 0;
    return ls;
}

list list_clone(list ls) {
    list ptr = list_init();
    *ptr = *ls;
    return ptr;
}

list list_get_elem(list ls, int idx) {
    list ptr = list_clone(ls);

    for (int i = 0; i < idx; i++) {
        ptr = ptr->next;
    }

    return ptr;
}

void list_set_elem(list ls, int idx, void *val) {
    list_get_elem(ls, idx)->val = val;
}

static void list_update_len(list ls) {
    list_elem ptr = list_clone(ls);

    int i = 1;
    while (ptr->next != NULL) {
        i++;
        ptr = ptr->next;
    }
    
    ls->len = i;
}

void list_add_elem(list ls, void *item) {
    if (ls->len != 0) {
        list_elem last = list_get_elem(ls, ls->len-1);

        list_elem elem = list_init();
        elem->val = item;
        elem->prev = last;
        ls->next = elem;
    } else {
        ls->val = item;
    }

    list_update_len(ls);
}

void list_remove_item(list ls, int idx) {
    list_elem del = list_get_elem(ls, idx);
    del->prev->next = del->next;
    free(del);

    list_update_len(del);
}

void free_list(list ls) {
    while (ls != NULL) {
        list next = ls->next;
        free(ls);
        ls = next;
    }
}

#ifdef __cplusplus
}
#endif