#ifndef MINIPL_SCANNER
#define MINIPL_SCANNER

#include "file.h"

#define empty_char   \
    ' ' : case '\t': \
    case '\v':       \
    case '\f':       \
    case '\r'

#define operator_or_bracket_except_asterisk \
    '(' : case ')':                         \
    case '-':                               \
    case '+':                               \
    case '=':                               \
    case '!':                               \
    case ';':                               \
    case '&':                               \
    case ':':                               \
    case '.'

#define operator_or_bracket_char         \
    operator_or_bracket_except_asterisk: \
    case '*'

#define letter_or_digit             \
    'a' ... 'z' : case 'A' ... 'Z': \
    case '0' ... '9'

typedef struct position {
    int line;
    int column;
} position;

typedef struct character {
    unsigned char value;
    position position;
    struct character* next;
    struct character* previous;
} character;

typedef struct state_context {
    minipl_contents reader;
    position position;
    character* writer;
} state_context;

typedef void state_function;
typedef state_function*(state)(state_context*);

character* minipl_screen(minipl_contents);

state_function* asterisk_in_multiline_comment_after_normal(state_context*);
state_function* asterisk_in_multiline_comment(state_context*);
state_function* beginning(state_context*);
state_function* empty_after_normal(state_context*);
state_function* empty(state_context*);
state_function* end_of_multiline_comment_after_normal(state_context*);
state_function* end_of_one_line_comment_after_normal(state_context*);
state_function* end_of_one_line_comment(state_context*);
state_function* end_of_quotation(state_context*);
state_function* escape_in_quotation(state_context*);
state_function* forward_slash_after_normal(state_context*);
state_function* forward_slash(state_context*);
state_function* multiline_comment_after_normal(state_context*);
state_function* multiline_comment(state_context*);
state_function* new_line_after_normal(state_context*);
state_function* new_line_in_multiline_comment_after_normal(state_context*);
state_function* new_line_in_multiline_comment(state_context*);
state_function* new_line_in_quotation(state_context*);
state_function* new_line(state_context*);
state_function* normal_after_normal(state_context*);
state_function* normal(state_context*);
state_function* one_line_comment(state_context*);
state_function* operator_or_bracket(state_context*);
state_function* operator_or_bracket(state_context*);
state_function* quotation_mark_in_quotation(state_context*);
state_function* quotation(state_context*);
state_function* start_multiline_comment_after_normal(state_context*);
state_function* start_one_line_comment_after_normal(state_context*);
state_function* start_one_line_comment(state_context*);

#endif
