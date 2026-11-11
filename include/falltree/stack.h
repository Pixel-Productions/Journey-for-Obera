#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

stack stack_init() {
    stack stk = (stack)calloc(1, sizeof(*stk));
    return stk;
}

void stack_push(stack stk, void *val) {
    stack_elem elem = (stack_elem)calloc(1, sizeof(*elem));
    elem->val = val;

    if (stk->head == NULL) {
        stk->head = elem;
    } else {
        stk->tail->next = elem;
    }

    stk->tail = elem;
}

void *stack_pop(stack stk) {
    void *val = stk->head->val;

    stack_elem next = stk->head->next;
    free(stk->head);
    stk->head = next;

    return val;
}

void *stack_get_elem(stack stk, int idx) {
    stack_elem ptr = stk->head;

    for (int i = 0; i < idx; i++) {
        ptr = ptr->next;
    }

    return ptr->val;
}

void free_stack(stack stk) {
    stack_elem ptr = stk->head;

    while (ptr != NULL) {
        stack_elem next = ptr->next;
        free(ptr);
        ptr = next;
    }

    free(stk);
}

#ifdef __cplusplus
}
#endif