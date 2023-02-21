#ifndef SCREENING_STATES
#define SCREENING_STATES

#include "state_context.h"

typedef void state_function;
typedef state_function*(state)(state_context*);

state_function* beginning(state_context*);

#endif
