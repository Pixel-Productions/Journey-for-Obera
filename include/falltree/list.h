#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

list list_init() {
    list ls = (list)calloc(1, sizeof(*ls));
    return ls;
}

static list_elem list_get_elem_struct(list ls, int idx) {
    list_elem ptr = ls->first;

    for (int i = 0; i < idx; i++) {
        ptr = ptr->next;
    }

    return ptr;
}

void *list_get_elem(list ls, int idx) {
    return list_get_elem_struct(ls, idx)->val;
}

void list_set_elem(list ls, int idx, void *val) {
    list_get_elem_struct(ls, idx)->val = val;
}

static void list_update_len(list ls) {
    if (ls->first != NULL) {
        int i = 1;
        for (list_elem ptr = ls->first; ptr->next != NULL; ptr = ptr->next)
            i++;
        
        ls->len = i;
    } else {
        ls->len = 0;
    }
}

void list_add_elem(list ls, void *val) {
    list_elem elem = (list_elem)calloc(1, sizeof(*elem));
    elem->val = val;

    if (ls->first == NULL) {
        ls->first = elem;
    } else {
        list_get_elem_struct(ls, ls->len-1)->next = elem;
    }

    list_update_len(ls);
}

void list_remove_elem(list ls, int idx) {
    list_elem elem = list_get_elem_struct(ls, idx);

    if (idx != 0) {
        list_elem prev = list_get_elem_struct(ls, idx-1);

        prev->next = elem->next;
    } else {
        ls->first = list_get_elem_struct(ls, 1);
    }

    free(elem);

    list_update_len(ls);
}

void free_list(list ls) {
    list_elem ptr = ls->first;

    while (ptr != NULL) {
        list_elem next = ptr->next;
        free(ptr);
        ptr = next;
    }

    free(ls);
}

#ifdef __cplusplus
}
#endif