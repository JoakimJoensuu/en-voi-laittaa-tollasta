#include "screener.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

void panic_unimplemented(const char* file, const int line, const char* function,
                         const state_context* context) {
    printf("%s:%d in function %s:\n", file, line, function);
    printf("Unimplemented character '%c' code: 0x%02X\n", *context->reader,
           *context->reader);
    exit(1);
}

position next_line(position current) {
    return (position){.line = current.line + 1, .column = 0};
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

character* store_value(char value, state_context* context) {
    character* next = malloc(sizeof(character));
    next->position  = context->position;
    next->next      = NULL;
    next->value     = value;
    next->previous  = context->writer;

    character* current = context->writer;
    current->next      = next;

    context->writer = next;

    return next;
}

character* store_current_value(state_context* context) {
    return store_value(*context->reader, context);
}

state_function* end_of_one_line_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case letter_or_digit:
            return &normal;
        case empty_char:
            return empty;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* one_line_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &end_of_one_line_comment;
        case '/':
        case empty_char:
        case letter_or_digit:
            return &one_line_comment;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* end_of_one_line_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_normal;
        case letter_or_digit:
            return &normal_after_normal;
        case empty_char:
            return empty_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* normal_after_normal(state_context* context) {
    store_value(' ', context);
    store_current_value(context);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal;
        case empty_char:
            return &empty_after_normal;
        case '\n':
            return &new_line_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* one_line_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &end_of_one_line_comment_after_normal;
        case '/':
        case empty_char:
        case letter_or_digit:
            return &one_line_comment_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* end_of_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case operator_separator_or_bracket_char:
            return &operator_or_bracket;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* quotation_mark_in_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &end_of_quotation;
        case letter_or_digit:
            return &quotation;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* escape_in_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case '\n':
            return new_line_in_quotation;
        case letter_or_digit:
        case operator_separator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* new_line_in_quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return new_line_in_quotation;
        case '"':
            return &end_of_quotation;
        case '\\':
            return escape_in_quotation;
        case letter_or_digit:
        case operator_separator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* quotation(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return new_line_in_quotation;
        case '"':
            return &end_of_quotation;
        case '\\':
            return escape_in_quotation;
        case '?':
        case ',':
        case letter_or_digit:
        case empty_char:
        case operator_separator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* operator_or_bracket(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal;
        case empty_char:
            return &empty;
        case '\n':
            return &new_line;
        case '/':
            return &forward_slash;
        case '"':
            return &quotation;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* new_line_after_normal(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case empty_char:
            return empty_after_normal;
        case '\n':
            return new_line_after_normal;
        case letter_or_digit:
            return normal_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* empty_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case empty_char:
            return empty_after_normal;
        case '\n':
            return new_line_after_normal;
        case letter_or_digit:
            return normal_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* multiline_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '*':
            return asterisk_in_multiline_comment_after_normal;
        case empty_char:
        case letter_or_digit:
            return multiline_comment_after_normal;
        case '\n':
            return &new_line_in_multiline_comment_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* asterisk_in_multiline_comment_after_normal(
    state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment_after_normal;
        case letter_or_digit:
            return &multiline_comment_after_normal;
        case '*':
            return &asterisk_in_multiline_comment_after_normal;
        case '/':
            return &end_of_multiline_comment_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* new_line_in_multiline_comment_after_normal(
    state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '\0':
            return NULL;
        case '*':
            return asterisk_in_multiline_comment_after_normal;
        case '/':
        case empty_char:
        case letter_or_digit:
        case operator_separator_or_bracket_except_asterisk:
            return multiline_comment_after_normal;
        case '\n':
            return new_line_in_multiline_comment_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* end_of_multiline_comment_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return new_line_after_normal;
        case letter_or_digit:
            return normal_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash_after_normal;
        case empty_char:
            return empty_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* forward_slash_after_normal(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '*':
            return multiline_comment_after_normal;
        case empty_char:
            return empty_after_forward_slash;
        case letter_or_digit:
            return normal_after_forward_slash;
        case '\n':
            return new_line_after_forward_slash;
        case '/':
            return one_line_comment_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* normal(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal;
        case empty_char:
            return &empty_after_normal;
        case '\n':
            return &new_line_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* end_of_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case letter_or_digit:
            return &normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash;
        case empty_char:
            return empty;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* asterisk_in_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case letter_or_digit:
            return &multiline_comment;
        case '*':
            return asterisk_in_multiline_comment;
        case '/':
            return end_of_multiline_comment;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* new_line_in_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '\0':
            return NULL;
        case '*':
            return asterisk_in_multiline_comment;
        case '/':
        case empty_char:
        case letter_or_digit:
        case operator_separator_or_bracket_except_asterisk:
            return &multiline_comment;
        case '\n':
            return &new_line_in_multiline_comment;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* multiline_comment(state_context* context) {
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case '*':
            return asterisk_in_multiline_comment;
        case empty_char:
        case letter_or_digit:
            return &multiline_comment;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* new_line_after_forward_slash(state_context* context) {
    store_value('/', context);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case '/':
            return &forward_slash;
        case letter_or_digit:
            return &normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* empty_after_forward_slash(state_context* context) {
    store_value('/', context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case '/':
            return &forward_slash;
        case letter_or_digit:
            return &normal;
        case '"':
            return quotation;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* normal_after_forward_slash(state_context* context) {
    store_value('/', context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal;
        case empty_char:
            return &empty_after_normal;
        case '\n':
            return &new_line_after_normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash_after_normal;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* forward_slash(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_forward_slash;
        case empty_char:
            return &empty_after_forward_slash;
        case '/':
            return &one_line_comment;
        case '*':
            return &multiline_comment;
        case letter_or_digit:
            return normal_after_forward_slash;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* empty(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case '/':
            return &forward_slash;
        case letter_or_digit:
            return &normal;
        case '"':
            return quotation;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* new_line(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case '/':
            return &forward_slash;
        case letter_or_digit:
            return &normal;
        case operator_separator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

state_function* beginning(state_context* context) {
    char first = *context->reader;

    context->position.line   = 1;
    context->position.column = 1;

    switch (first) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty;
        case letter_or_digit:
            return &normal;
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
    }
    panic_unimplemented(__FILE__, __LINE__, __func__, context);
    return NULL;
}

character* minipl_screen(minipl_contents contents) {
    character* results = calloc(1, sizeof(character));

    state_context* context = &(state_context){
        .reader   = contents,
        .writer   = results,
        .position = {0},
    };

    state* current_state = &beginning;

    while ((current_state = current_state(context)) != NULL)
        ;

    return results;
}
