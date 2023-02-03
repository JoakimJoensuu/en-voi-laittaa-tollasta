#include "scanner.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

void panic_unimplemented_char(const char* function,
                              const state_context* context) {
    printf("\n%s: Unimplemented character '%c' code: 0x%02X\n", function,
           *context->reader, *context->reader);
    printf("Characters already scanned:\n");
    printf("\"\"\"\n!");
    character* next = context->scanned->next;
    free(context->scanned);
    while (next != NULL) {
        printf("%c", next->value);
        character* previous = next;
        next = previous->next;
        free(previous);
    }
    printf("!\n\"\"\"\n\n");

    free(context->data);
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

char remove_value(character** writer) {
    if (*writer == (*writer)->previous) {
        printf("Error, removing head\n");
        exit(1);
    }

    char removed = (*writer)->value;

    character* previous = (*writer)->previous;
    previous->next = NULL;

    free(*writer);

    *writer = previous;

    return removed;
}

char store_value(char value, position position, character** writer) {
    (*writer)->next = malloc(sizeof(character));
    character* next = (*writer)->next;
    next->previous = *writer;
    *writer = next;

    (*writer)->position = position;
    (*writer)->next = NULL;
    (*writer)->value = value;

    return value;
}

char store_current_value(state_context* context) {
    return store_value(*context->reader, context->position, &context->writer);
}

state_function* end_of_one_line_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case letter_or_digit:
            return &normal_character;
    }
    panic_unimplemented_char(__func__, context);
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
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* start_one_line_comment(state_context* context) {
    remove_value(&context->writer);
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
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* operator_or_bracket(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case '.':
        case operator_or_bracket_char:
            return operator_or_bracket;
        case '\n':
            return &new_line;
        case empty_char:
            return empty_character;
        case letter_or_digit:
            return normal_character;
    }
    panic_unimplemented_char(__func__, context);
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
        case operator_or_bracket_char:
            return &operator_or_bracket;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty_character;
    }
    panic_unimplemented_char(__func__, context);
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
    panic_unimplemented_char(__func__, context);
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
        case operator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented_char(__func__, context);
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
        case operator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented_char(__func__, context);
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
        case '.':
        case '?':
        case ',':
        case letter_or_digit:
        case empty_char:
        case operator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* operator_or_bracket_after_whitespaces_and_normal_char(
    state_context* context) {
    remove_value(&context->writer);
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal_character;
        case empty_char:
            return &empty_character;
        case '\n':
            return &new_line;
        case '/':
            return &forward_slash;
        case operator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* normal_after_whitespaces_and_normal_char(
    state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal_character;
        case empty_char:
            return &first_empty_char_after_normal_character;
        case '\n':
            return &first_new_line_after_normal_character;
        case '.':
        case operator_or_bracket_char:
            return &operator_or_bracket;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* new_line_after_normal_character(state_context* context) {
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case empty_char:
            return empty_char_after_normal_character;
        case '\n':
            return new_line_after_normal_character;
        case letter_or_digit:
            return normal_after_whitespaces_and_normal_char;
        case operator_or_bracket_char:
            return operator_or_bracket_after_whitespaces_and_normal_char;
        case '/':
            return forward_slash;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* empty_char_after_normal_character(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case empty_char:
            return empty_char_after_normal_character;
        case '\n':
            return new_line_after_normal_character;
        case letter_or_digit:
            return normal_after_whitespaces_and_normal_char;
        case operator_or_bracket_char:
            return operator_or_bracket_after_whitespaces_and_normal_char;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* first_new_line_after_normal_character(state_context* context) {
    store_value(' ', context->position, &context->writer);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation;
        case empty_char:
            return empty_char_after_normal_character;
        case '\n':
            return new_line_after_normal_character;
        case letter_or_digit:
            return normal_after_whitespaces_and_normal_char;
        case operator_or_bracket_char:
            return operator_or_bracket_after_whitespaces_and_normal_char;
        case '/':
            return forward_slash;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* quotation_after_whitespaces_and_normal_char(
    state_context* context) {
    remove_value(&context->writer);
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
        case letter_or_digit:
        case empty_char:
        case operator_or_bracket_char:
            return &quotation;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* first_empty_char_after_normal_character(
    state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '"':
            return &quotation_after_whitespaces_and_normal_char;
        case empty_char:
            return empty_char_after_normal_character;
        case '\n':
            return new_line_after_normal_character;
        case letter_or_digit:
            return normal_after_whitespaces_and_normal_char;
        case operator_or_bracket_char:
            return operator_or_bracket_after_whitespaces_and_normal_char;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* normal_character(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case letter_or_digit:
            return &normal_character;
        case empty_char:
            return &first_empty_char_after_normal_character;
        case '\n':
            return &first_new_line_after_normal_character;
        case operator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* new_line_after_single_forward_slash(state_context* context) {
    remove_value(&context->writer);
    unsigned char next = move_to_next_line(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return new_line;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* empty_after_single_forward_slash(state_context* context) {
    remove_value(&context->writer);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
    }
    panic_unimplemented_char(__func__, context);
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
            return &normal_character;
        case operator_or_bracket_char:
            return operator_or_bracket;
        case '/':
            return forward_slash;
        case empty_char:
            return empty_character;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* asterisk_in_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);
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
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* new_line_in_multiline_comment(state_context* context) {
    unsigned char next = move_to_next_line(context);
    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case '*':
            return asterisk_in_multiline_comment;
        case '/':
        case empty_char:
        case letter_or_digit:
            return &multiline_comment;
    }
    panic_unimplemented_char(__func__, context);
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
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* beginning_of_multiline_comment(state_context* context) {
    remove_value(&context->writer);
    unsigned char next = move_to_next_column(context);
    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_in_multiline_comment;
        case '*':
            return asterisk_in_multiline_comment;
        case letter_or_digit:
            return &multiline_comment;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* forward_slash(state_context* context) {
    store_current_value(context);
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line_after_single_forward_slash;
        case empty_char:
            return &empty_after_single_forward_slash;
        case '/':
            return &start_one_line_comment;
        case '*':
            return &beginning_of_multiline_comment;
        case letter_or_digit:
            return normal_character;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* empty_character(state_context* context) {
    unsigned char next = move_to_next_column(context);

    switch (next) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty_character;
        case '/':
            return &forward_slash;
        case letter_or_digit:
            return &normal_character;
        case '"':
            return quotation;
        case operator_or_bracket_char:
            return operator_or_bracket;
    }
    panic_unimplemented_char(__func__, context);
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
            return &empty_character;
        case '/':
            return &forward_slash;
        case letter_or_digit:
            return &normal_character;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

state_function* beginning(state_context* context) {
    char first = *context->reader;

    context->position.line = 1;
    context->position.column = 1;
    context->scanned = context->writer = malloc(sizeof(character));
    context->scanned->next = context->writer->next = NULL;
    context->scanned->previous = context->writer->previous = context->writer;

    switch (first) {
        case '\0':
            return NULL;
        case '\n':
            return &new_line;
        case empty_char:
            return &empty_character;
        case letter_or_digit:
            return &normal_character;
        case '"':
            return &quotation;
        case '/':
            return &forward_slash;
    }
    panic_unimplemented_char(__func__, context);
    return NULL;
}

character* minipl_scan(minipl_contents contents) {
    state_context* context = &(state_context){
        .data = contents,
        .reader = contents,
        .writer = NULL,
        .scanned = NULL,
    };

    state* current_state = &beginning;

    while (current_state != NULL) {
        /*
        int size = 0;
        character* temp = context->scanned;
        while (temp != context->writer) {
            temp = temp->next;
            size++;
        }
        printf("%d%c ", size, *context->reader);
        */
        state* next = (state*)current_state(context);
        current_state = next;
    }
    printf("\n");

    return context->scanned;
}
