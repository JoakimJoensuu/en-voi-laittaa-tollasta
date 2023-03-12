#ifndef SCREENING_STATE_CONTEXT
#define SCREENING_STATE_CONTEXT

#include "../opening/file.h"
#include "screener.h"

typedef struct state_context {
    char*       reader;
    position    position;
    characters* writer;
} state_context;

position next_line(position current);
position next_column(position current);

unsigned char move_to_next_line(state_context* context);
unsigned char move_to_next_column(state_context* context);

void store_value(char value, state_context* context);
void store_current_value(state_context* context);

state_context* init_state_context(state_context* context, char* contents,
                                  characters* results);

#endif
