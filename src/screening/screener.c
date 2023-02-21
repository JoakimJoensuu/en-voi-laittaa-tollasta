#include "screener.h"

#include <stddef.h>

#include "../file.h"
#include "state_context.h"
#include "states.h"

character* minipl_screen(minipl_contents contents) {
    character* results = new_characters();

    state_context* context =
        init_state_context(&(state_context){}, contents, results);

    state* current_state = &beginning;

    while (current_state != NULL) {
        current_state = current_state(context);
    }

    return results;
}
