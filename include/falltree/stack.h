#include <stdlib.h>
#include "types.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

stack stack_init() {
    stack stk = (stack)calloc(1, sizeof(*stk));

    stk->head = stk;
    stk->tail = stk;

    return stk;
}

void stack_push(stack stk, void *elem) {
    stack_elem stk_elem = stack_init();

    stk->tail->next = stk_elem;

    stk_elem->head = stk;
    stk_elem->val = elem;

    stk->tail = stk_elem;
}

void *stack_pop(stack stk) {
    void *val = stk->val;

    stack_elem head = stk->head;
    stk->head = stk->next;
    
    free(head);

    return val;
}

#ifdef __cplusplus
}
#endif