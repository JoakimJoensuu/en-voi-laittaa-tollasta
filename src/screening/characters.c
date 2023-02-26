#include "characters.h"

#include <stdlib.h>
#include <string.h>

#define SUCCESS 0

int enlarge_char_list(characters* target) {
    int       new_size      = target->size * 2;
    char*     new_values    = malloc(new_size * sizeof(char));
    position* new_positions = malloc(new_size * sizeof(position));

    if (new_values == NULL || new_positions == NULL) {
        return 1;
    }

    memcpy(new_values, target->values, target->size * sizeof(char));
    memcpy(new_positions, target->positions, target->size * sizeof(position));
    free(target->values);
    free(target->positions);
    target->values    = new_values;
    target->positions = new_positions;

    target->size *= 2;

    return SUCCESS;
}

int append_char(characters* target, char appendable, position pos) {
    if (target->size == target->length) {
        if (enlarge_char_list(target) != SUCCESS) {
            return 1;
        }
    }

    target->values[target->length]    = appendable;
    target->positions[target->length] = pos;
    target->length++;

    return SUCCESS;
}

characters* new_characters() {
    characters* new = malloc(sizeof(characters));

    *new = (characters){
        .values    = malloc(sizeof(char) * 1),
        .length    = 0,
        .size      = 1,
        .positions = malloc(sizeof(position) * 1),
    };

    return new;
}
