#include "ast.h"

#include <stdlib.h>

#define SUCCESS 0

#include <string.h>

int enlarge_child_list(node* target) {
    int new_size = target->children_size * 2;

    node** new_children = malloc(new_size * sizeof(*new_children));

    if (new_children == NULL) {
        return 1;
    }

    memcpy(new_children, target->children,
           target->children_count * sizeof(*target->children));
    free(target->children);
    target->children = new_children;

    target->children_size *= 2;

    return SUCCESS;
}

int add_child(node* parent, node* child) {
    if (parent->children_size == parent->children_count) {
        if (enlarge_child_list(parent) != SUCCESS) {
            return 1;
        }
    }

    parent->children[parent->children_count] = child;
    parent->children_count++;

    return SUCCESS;
}

node* new_node(node_type type) {
    node* new = malloc(sizeof(*new));

    *new = (node){
        .type           = type,
        .children       = malloc(sizeof(*new) * 1),
        .children_count = 0,
        .children_size  = 1,
        .data_size      = 0,
        .data           = NULL,
    };

    return new;
}
