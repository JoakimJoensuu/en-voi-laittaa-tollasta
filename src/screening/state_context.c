#include "state_context.h"

#include <stddef.h>
#include <stdlib.h>

position next_line(position current) {
    return (position){.line = current.line + 1, .column = 0};
}

unsigned char move_to_next_line(state_context* context) {
    context->position = next_line(context->position);
    context->reader++;
    return *context->reader;
}

position next_column(position current) {
    return (position){.line = current.line, .column = current.column + 1};
}

unsigned char move_to_next_column(state_context* context) {
    context->position = next_column(context->position);
    context->reader++;
    return *context->reader;
}

character* store_value(char value, state_context* context) {
    character* next = malloc(sizeof(character));
    next->position  = context->position;
    next->next      = NULL;
    next->value     = value;

    context->writer->next = next;

    context->writer = next;

    return next;
}

character* store_current_value(state_context* context) {
    return store_value(*context->reader, context);
}

state_context* init_state_context(state_context*  context,
                                  minipl_contents contents,
                                  character*      results) {
    *context = (state_context){
        .reader = contents,
        .writer = results,
        .position =
            {
                .column = 1,
                .line   = 1,
            },
    };

    return context;
};
