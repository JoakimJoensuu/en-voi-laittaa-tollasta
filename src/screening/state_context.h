#ifndef SCREENING_STATE_CONTEXT
#define SCREENING_STATE_CONTEXT

#include "../file.h"
#include "screener.h"

typedef struct state_context {
    minipl_contents reader;
    position        position;
    character*      writer;
} state_context;

position next_line(position current);
position next_column(position current);

unsigned char move_to_next_line(state_context* context);
unsigned char move_to_next_column(state_context* context);

character* store_value(char value, state_context* context);
character* store_current_value(state_context* context);

state_context* init_state_context(state_context*  context,
                                  minipl_contents contents, character* results);

#endif
