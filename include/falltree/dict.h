#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

dict dict_init() {
    dict dct = (dict)calloc(1, sizeof(*dct));
    return dct;
}

static dict_elem dict_get_elem_struct(dict dct, void *key) {
    for (dict_elem ptr = dct->first; ptr != NULL; ptr = ptr->next) {
        if (ptr->key == key)
            return ptr;
    }

    return NULL;
}

void *dict_get_elem(dict dct, void *key) {
    return dict_get_elem_struct(dct, key)->val;
}

static dict_elem dict_get_elem_struct_idx(dict dct, int idx) {
    dict_elem ptr = dct->first;

    for (int i = 0; i < idx; i++) {
        ptr = ptr->next;
    }

    return ptr;
}

void *dict_get_elem_idx(dict dct, int idx) {
    return dict_get_elem_struct_idx(dct, idx)->val;
}

void dict_set_elem(dict dct, void *key, void *val) {
    dict_elem elem = dict_get_elem_struct(dct, key);
    elem->key = key;
    elem->val = val;
}

static void dict_update_len(dict dct) {
    int i = 1;
    for (dict_elem ptr = dct->first; ptr->next != NULL; ptr = ptr->next)
        i++;

    dct->len = i;
}

void dict_add_elem(dict dct, void *key, void *val) {
    dict_elem elem = (dict_elem)calloc(1, sizeof(*elem));
    elem->val = val;

    if (dct->first == NULL) {
        dct->first = elem;
    } else {
        dict_get_elem_struct_idx(dct, dct->len-1)->next = elem;
    }

    dict_update_len(dct);
}

void dict_remove_elem(dict dct, void *key) {
    dict_elem elem = dict_get_elem_struct(dct, key);

    int idx = 0;
    for (dict_elem ptr = dct->first; ptr->key != key; ptr = ptr->next) idx++; 

    if (idx != 0) {
        dict_elem prev = dict_get_elem_struct_idx(dct, idx-1);

        prev->next = elem->next;
    } else {
        dct->first = dict_get_elem_struct_idx(dct, 1);
    }

    free(elem);

    dict_update_len(dct);
}

void free_dict(dict dct) {
    dict_elem ptr = dct->first;

    while (ptr != NULL) {
        dict_elem next = ptr->next;
        free(ptr);
        ptr = next;
    }

    free(dct);
}

#ifdef __cplusplus
}
#endif