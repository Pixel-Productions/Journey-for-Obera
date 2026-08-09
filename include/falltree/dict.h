#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

dict dict_init() {
    dict dct = (dict)calloc(1, sizeof(*dct));
    dct->len = 0;
    return dct;
}

dict dict_clone(dict dct) {
    dict ptr = dict_init();
    *ptr = *dct;
    return ptr;
}

dict_elem dict_get_elem(dict dct, void *key) {
    for (dict ptr = dct; ptr != NULL; ptr = ptr->next) {
        if (ptr->key == key)
            return ptr;
    }

    return NULL;
}

dict_elem dict_get_elem_idx(dict dct, int idx) {
    dict ptr = dct;

    for (int i = 0; i < idx; i++) {
        ptr = ptr->next;
    }

    return ptr;
}

void dict_set_elem(dict dct, void *key, void *val) {
    dict_elem elem = dict_get_elem(dct, key);
    elem->key = key;
    elem->val = val;
}

static void dict_update_len(dict dct) {
    int i = 1;
    for (dict ptr = dct; ptr->next != NULL; ptr = ptr->next) {
        i++;
    }

    dct->len = i;
}

void dict_add_elem(dict dct, void *key, void *val) {
    if (dct->len != 0) {
        dict_elem last = dict_get_elem_idx(dct, dct->len-1);

        dict_elem elem = dict_init();
        elem->key = key;
        elem->val = val;
        elem->prev = last;
        last->next = elem;
    } else {
        dct->key = key;
        dct->val = val;
    }

    dict_update_len(dct);
}

void dict_remove_elem(dict dct, void *key) {
    dict_elem del = dict_get_elem(dct, key);
    del->prev->next = del->next;
    free(del);

    dict_update_len(dct);
}

void free_dict(dict dct) {
    while (dct != NULL) {
        dict next = dct->next;
        free(dct);
        dct = next;
    }
}

#ifdef __cplusplus
}
#endif