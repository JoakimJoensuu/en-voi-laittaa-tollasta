#include "tokens.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUCCESS 0

int enlarge_token_list(tokens* target) {
    int new_size = target->size * 2;

    token* new_values = malloc(new_size * sizeof(token));

    if (new_values == NULL) {
        return 1;
    }

    memcpy(new_values, target->values, target->size * sizeof(token));
    free(target->values);
    target->values = new_values;

    target->size *= 2;

    return SUCCESS;
}

int append_tokens(tokens* target, token appendable) {
    if (target->size == target->length) {
        if (enlarge_token_list(target) != SUCCESS) {
            return 1;
        }
    }

    target->values[target->length] = appendable;
    target->length++;

    return SUCCESS;
}

tokens* new_tokens() {
    tokens* new = malloc(sizeof(tokens));

    *new = (tokens){
        .values = malloc(sizeof(token) * 1),
        .length = 0,
        .size   = 1,
    };

    return new;
}
