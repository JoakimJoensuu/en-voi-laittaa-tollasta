#include "states.h"

#include <stdio.h>
#include <stdlib.h>

#include "../opening/file.h"
#include "screener.h"
#include "state_context.h"

#define empty_char ' ' : case '\t' : case '\v' : case '\f' : case '\r'

#define operator_or_separator_except_asterisk                             \
    '(' : case ')' : case '-' : case '+' : case '=' : case '!' : case ';' \
        : case '&' : case ':' : case '.' : case '>' : case '<'

#define operator_or_separator_char         \
    operator_or_separator_except_asterisk: \
    case '*'

#define letter_or_digit 'a' ... 'z' : case 'A' ... 'Z' : case '0' ... '9'

#define PANIC_UNIMPLEMENTED(CHARACTER, POSITION)                            \
    do {                                                                    \
        printf("%s:%d in function %s:\n", __FILE__, __LINE__, __func__);    \
        printf(                                                             \
            "Unimplemented character '%c' code: 0x%02X at line %d, column " \
            "%d\n",                                                         \
            CHARACTER, CHARACTER, POSITION.line, POSITION.column);          \
        exit(1);                                                            \
    } while (0)

state_function* asterisk_in_multiline_comment_after_normal(state_context*);
state_function* asterisk_in_multiline_comment(state_context*);
state_function* empty_after_forward_slash(state_context*);
state_function* empty_after_normal(state_context*);
state_function* empty(state_context*);
state_function* end_of_multiline_comment_after_normal(state_context*);
state_function* end_of_multiline_comment(state_context*);
state_function* end_of_one_line_comment_after_normal(state_context*);
state_function* end_of_one_line_comment(state_context*);
state_function* end_of_quotation(state_context*);
state_function* escape_in_quotation(state_context*);
state_function* forward_slash_after_normal(state_context*);
state_function* forward_slash(state_context*);
state_function* multiline_comment_after_normal(state_context*);
state_function* multiline_comment(state_context*);
state_function* new_line_after_forward_slash(state_context*);
state_function* new_line_after_normal(state_context*);
state_function* new_line_in_multiline_comment_after_normal(state_context*);
state_function* new_line_in_multiline_comment(state_context*);
state_function* new_line_in_quotation(state_context*);
state_function* new_line(state_context*);
state_function* normal_after_forward_slash(state_context*);
state_function* normal_after_normal(state_context*);
state_function* normal(state_context*);
state_function* one_line_comment_after_normal(state_context*);
state_function* one_line_comment(state_context*);
state_function* operator_or_separator(state_context*);
state_function* quotation_mark_in_quotation(state_context*);
state_function* quotation(state_context*);

state_function* one_line_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '/':
            return &one_line_comment;
        case '\0':
            return NULL;
        case '\n':
            return &end_of_one_line_comment;
        case empty_char:
            return &one_line_comment;
        case letter_or_digit:
            return &one_line_comment;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* one_line_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '/':
            return &one_line_comment_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &end_of_one_line_comment_after_normal;
        case empty_char:
            return &one_line_comment_after_normal;
        case letter_or_digit:
            return &one_line_comment_after_normal;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* quotation_mark_in_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '"':
            return &end_of_quotation;
        case '\0':
            return NULL;
        case letter_or_digit:
            return &quotation;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* escape_in_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '"':
            return &quotation;
        case '\\':
            return &escape_in_quotation;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_quotation;
        case letter_or_digit:
            return &quotation;
        case operator_or_separator_char:
            return &quotation;
        case ' ':
            return &quotation;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* multiline_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '*':
            return &asterisk_in_multiline_comment_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment_after_normal;
        case empty_char:
            return &multiline_comment_after_normal;
        case letter_or_digit:
            return &multiline_comment_after_normal;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* asterisk_in_multiline_comment_after_normal(
    state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '*':
            return &asterisk_in_multiline_comment_after_normal;
        case '/':
            return &end_of_multiline_comment_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment_after_normal;
        case letter_or_digit:
            return &multiline_comment_after_normal;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* new_line_in_multiline_comment_after_normal(
    state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '*':
            return &asterisk_in_multiline_comment_after_normal;
        case '/':
            return &multiline_comment_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment_after_normal;
        case empty_char:
            return &multiline_comment_after_normal;
        case letter_or_digit:
            return &multiline_comment_after_normal;
        case operator_or_separator_except_asterisk:
            return &multiline_comment_after_normal;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* forward_slash_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '*':
            return &multiline_comment_after_normal;
        case '/':
            return &one_line_comment_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_forward_slash;
        case empty_char:
            return &empty_after_forward_slash;
        case letter_or_digit:
            return &normal_after_forward_slash;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* asterisk_in_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '*':
            return &asterisk_in_multiline_comment;
        case '/':
            return &end_of_multiline_comment;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case letter_or_digit:
            return &multiline_comment;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* new_line_in_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '*':
            return &asterisk_in_multiline_comment;
        case '/':
            return &multiline_comment;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case empty_char:
            return &multiline_comment;
        case letter_or_digit:
            return &multiline_comment;
        case operator_or_separator_except_asterisk:
            return &multiline_comment;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* multiline_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '*':
            return &asterisk_in_multiline_comment;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case empty_char:
            return &multiline_comment;
        case letter_or_digit:
            return &multiline_comment;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* forward_slash(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '*':
            return &multiline_comment;
        case '/':
            return &one_line_comment;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_forward_slash;
        case empty_char:
            return &empty_after_forward_slash;
        case letter_or_digit:
            return &normal_after_forward_slash;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* new_line_in_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case ',':
            return &quotation;
        case '?':
            return &quotation;
        case '"':
            return &end_of_quotation;
        case '\\':
            return &escape_in_quotation;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_quotation;
        case empty_char:
            return &quotation;
        case letter_or_digit:
            return &quotation;
        case operator_or_separator_char:
            return &quotation;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case ',':
            return &quotation;
        case '?':
            return &quotation;
        case '"':
            return &end_of_quotation;
        case '\\':
            return &escape_in_quotation;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_quotation;
        case empty_char:
            return &quotation;
        case letter_or_digit:
            return &quotation;
        case operator_or_separator_char:
            return &quotation;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* end_of_one_line_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal_after_normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* new_line_after_normal(state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal_after_normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* empty_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal_after_normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* end_of_multiline_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal_after_normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* normal_after_normal(state_context* context) {
    store_value(' ', context);
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* normal(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* normal_after_forward_slash(state_context* context) {
    store_value('/', context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '/':
            return &forward_slash_after_normal;
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case empty_char:
            return &empty_after_normal;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* end_of_one_line_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* end_of_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* operator_or_separator(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* end_of_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* new_line_after_forward_slash(state_context* context) {
    store_value('/', context);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* empty_after_forward_slash(state_context* context) {
    store_value('/', context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* empty(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* new_line(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}

state_function* beginning(state_context* context) {
    char next = *context->reader;

    switch (next) {
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case operator_or_separator_char:
            return &operator_or_separator;
        default:
            PANIC_UNIMPLEMENTED(next, context->position);
    }
}
