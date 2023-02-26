#include "state_context.h"

#include <stddef.h>
#include <stdlib.h>

#define SUCCESS 0

position next_line(position current) {
    return (position){.line = current.line + 1, .column = 1};
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

void store_value(char value, state_context* context) {
    append_char(context->writer, value, context->position);
}

void store_current_value(state_context* context) {
    store_value(*context->reader, context);
}

state_context* init_state_context(state_context* context, char* contents,
                                  characters* results) {
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
