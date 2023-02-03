#ifndef MINIPL_SCANNER
#define MINIPL_SCANNER

#include "file.h"

#define empty_char   \
    ' ' : case '\t': \
    case '\v':       \
    case '\f':       \
    case '\r'

#define operator_or_bracket_char \
    '(' : case ')':              \
    case '*':                    \
    case '-':                    \
    case '+':                    \
    case '=':                    \
    case '!':                    \
    case ';':                    \
    case '&':                    \
    case ':'

#define letter_or_digit             \
    'a' ... 'z' : case 'A' ... 'Z': \
    case '0' ... '9'

#define any_char_except_escape_null_newline_or_quotation \
    1 ... 9 : case 11 ... 33:                            \
    case 35 ... 91:                                      \
    case 93 ... 255

#define any_char_except_null_or_newline 1 ... 9 : case 11 ... 255

#define any_char_except_null_quotation_or_newline \
    1 ... 9 : case 11 ... 33:                     \
    case 35 ... 255

#define any_char_except_null_quotation_or_escape \
    1 ... 33 : case 35 ... 91:                   \
    case 93 ... 255

#define any_char_except_null_new_line_quotation_or_escape \
    1 ... 9 : case 11 ... 33:                             \
    case 35 ... 91:                                       \
    case 93 ... 255

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
    minipl_contents data;
    minipl_contents reader;
    position position;
    character* writer;
    character* scanned;
} state_context;

typedef void state_function;
typedef state_function*(state)(state_context*);

character* minipl_scan(minipl_contents);

state_function* beginning(state_context*);
state_function* empty_char_after_normal_character(state_context*);
state_function* empty_character(state_context*);
state_function* end_of_quotation(state_context*);
state_function* first_empty_char_after_normal_character(state_context*);
state_function* first_new_line_after_normal_character(state_context*);
state_function* forward_slash(state_context*);
state_function* multiline_comment(state_context*);
state_function* new_line_after_normal_character(state_context*);
state_function* new_line_in_multiline_comment(state_context*);
state_function* new_line_in_quotation(state_context*);
state_function* new_line(state_context*);
state_function* normal_after_whitespaces_and_normal_char(state_context*);
state_function* normal_character(state_context*);
state_function* one_line_comment(state_context*);
state_function* operator_or_bracket(state_context*);
state_function* quotation(state_context*);

#endif
