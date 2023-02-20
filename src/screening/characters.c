#include "characters.h"

#include <stdlib.h>

character* new_characters() {
    character* new = malloc(sizeof(character));

    *new = (character){
        .next = NULL,
        .position =
            {
                .column = 0,
                .line   = 0,
            },
        .value = '\0',
    };

    return new;
}
