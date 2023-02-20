#ifndef SCREENING_STATES
#define SCREENING_STATES

#include <stdio.h>
#include <stdlib.h>

#include "state_context.h"

#define empty_char ' ' : case '\t' : case '\v' : case '\f' : case '\r'

#define operator_separator_or_bracket_except_asterisk                     \
    '(' : case ')' : case '-' : case '+' : case '=' : case '!' : case ';' \
        : case '&' : case ':' : case '.'

#define operator_separator_or_bracket_char         \
    operator_separator_or_bracket_except_asterisk: \
    case '*'

#define letter_or_digit 'a' ... 'z' : case 'A' ... 'Z' : case '0' ... '9'

typedef void state_function;
typedef state_function*(state)(state_context*);

state_function* beginning(state_context*);

#endif
