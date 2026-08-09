#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

list list_init() {
    list ls = (list)calloc(1, sizeof*ls);
    ls->len = 0;
    return ls;
}

list list_clone(list ls) {
    list ptr = list_init();
    *ptr = *ls;
    return ptr;
}

list list_get_elem(list ls, int idx) {
    list ptr = ls;

    for (int i = 0; i < idx; i++) {
        ptr = ptr->next;
    }

    return ptr;
}

void list_set_elem(list ls, int idx, void *val) {
    list_get_elem(ls, idx)->val = val;
}

static void list_update_len(list ls) {
    int i = 1;
    for (list ptr = ls; ptr->next != NULL; ptr = ptr->next) {
        i++;
    }
    
    ls->len = i;
}

void list_add_elem(list ls, void *item) {
    if (ls->len != 0) {
        list_elem last = list_get_elem(ls, ls->len-1);

        list_elem elem = list_init();
        elem->val = item;
        elem->prev = last;
        last->next = elem;
    } else {
        ls->val = item;
    }

    list_update_len(ls);
}

void list_remove_elem(list ls, int idx) {
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