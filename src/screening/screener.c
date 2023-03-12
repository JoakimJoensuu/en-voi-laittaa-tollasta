#include "screener.h"

#include <stddef.h>

#include "../opening/file.h"
#include "state_context.h"
#include "states.h"

characters* screen(char* source_code) {
    characters* results = new_characters();

    state_context* context =
        init_state_context(&(state_context){}, source_code, results);

    state* current_state = &beginning;

    while (current_state != NULL) {
        current_state = current_state(context);
    }

    return results;
}
