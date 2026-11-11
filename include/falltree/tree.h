#include <stdlib.h>
#include "types.h"
#include "list.h"

#pragma once

node_tree node_tree_init() {
    node_tree tree = (node_tree)malloc(sizeof(*tree));
    
    tree->children = list_init();
    
    return tree;
}

node node_init(void *val) {
    node nd = (node)malloc(sizeof(*nd));
    
    nd->val = val;
    nd->children = list_init();

    return nd;
}

void node_tree_add_child(node_tree tree, void *val) {
    list_add_elem(tree->children, node_init(val));
}

void node_add_child(node nd, void *val) {
    list_add_elem(nd->children, node_init(val));
}

void node_tree_remove_child(node_tree tree, int idx) {
    node child = (node)list_get_elem(tree->children, idx);
    free(child);

    list_remove_elem(tree->children, idx);
}

void node_remove_child(node nd, int idx) {
    node child = (node)list_get_elem(nd->children, idx);
    free(child);

    list_remove_elem(nd->children, idx);
}

void *node_tree_get_child(node_tree tree, int idx) {
    return ((node)list_get_elem(tree->children, idx))->val;
}

void *node_get_child(node nd, int idx) {
    return ((node)list_get_elem(nd->children, idx))->val;
}

void node_tree_set_child(node_tree tree, int idx, void *val) {
    ((node)list_get_elem(tree->children, idx))->val = val;
}

void node_set_child(node nd, int idx, void *val) {
    ((node)list_get_elem(nd->children, idx))->val = val;
}