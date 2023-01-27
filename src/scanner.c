
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "scanner.h"

typedef struct {
    int row;
    int column;
    char data;
} character;

state one_line_comment(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case '\n':
            next.compute = &normal_character;
            break;
        default:
            next.compute = &one_line_comment;
            break;
    }

    return next;
}

state asterisk_in_multiline_comment(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case '/':
            next.compute = &normal_character;
            break;
        default:
            next.compute = &multiline_comment;
            break;
    }

    return next;
}

state multiline_comment(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case '*':
            next.compute = &asterisk_in_multiline_comment;
            break;
        default:
            next.compute = &multiline_comment;
            break;
    }

    return next;
}

state first_forward_slash(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case '*':
            next.compute = &multiline_comment;
            break;
        case '/':
            next.compute = &one_line_comment;
            break;
        case whitespace:
            printf("/");
            next.compute = &empty_char;
            break;
        default:
            printf("/");
            printf("%c", *current.data);
            next.compute = &empty_char;
            break;
    }

    return next;
}

state empty_char_after_normal(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case whitespace:
            next.compute = &empty_char_after_normal;
            break;
        case '/':
            next.compute = &first_forward_slash;
            break;
        case '"':
            printf("%c", *current.data);
            next.compute = &quotation;
            break;
        case operator_or_bracket_char:
            printf("%c", *current.data);
            next.compute = &operator_or_bracket;
            break;
        default:
            printf(" ");
            printf("%c", *current.data);
            next.compute = &normal_character;
            break;
    }

    return next;
}

state normal_character(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case whitespace:
            next.compute = &empty_char_after_normal;
            break;
        case '/':
            next.compute = &first_forward_slash;
            break;
        case '"':
            printf("%c", *current.data);
            next.compute = &quotation;
            break;
        case operator_or_bracket_char:
            printf("%c", *current.data);
            next.compute = &operator_or_bracket;
            break;
        default:
            printf("%c", *current.data);
            next.compute = &normal_character;
            break;
    }

    return next;
}

state quotation_end(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case whitespace:
            next.compute = &empty_char;
            break;
        default:
            printf("%c", *current.data);
            next.compute = normal_character;
            break;
    }
    return next;
}

state quotation(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case '"':
            printf("%c", *current.data);
            next.compute = &quotation_end;
            break;
        default:
            printf("%c", *current.data);
            next.compute = &quotation;
            break;
    }

    return next;
}

state operator_or_bracket(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case whitespace:
            next.compute = &empty_char;
            break;
        case '/':
            next.compute = &first_forward_slash;
            break;
        case '"':
            printf("%c", *current.data);
            next.compute = &quotation;
            break;
        case operator_or_bracket_char:
            printf("%c", *current.data);
            next.compute = &operator_or_bracket;
            break;
        default:
            printf("%c", *current.data);
            next.compute = &normal_character;
            break;
    }

    return next;
}

state empty_char(state_context current)
{
    state next = { .context = {
                       .data = current.data + 1,
                   } };

    switch (*current.data) {
        case '\0':
            next.compute = NULL;
            break;
        case whitespace:
            next.compute = &empty_char;
            break;
        case '/':
            next.compute = &first_forward_slash;
            break;
        case '"':
            printf("%c", *current.data);
            next.compute = &quotation;
            break;
        case operator_or_bracket_char:
            printf("%c", *current.data);
            next.compute = &operator_or_bracket;
            break;
        default:
            printf("%c", *current.data);
            next.compute = &normal_character;
            break;
    }

    return next;
}

int minipl_scan(minipl_contents contents)
{
    state current = {
        .compute = &empty_char,
        .context = { .data = contents.data,
                     .line = 0,
                     .column = 0 },
    };

    while (current.compute != NULL) {
        state next = current.compute(current.context);
        current = next;
    }

    printf("\n");

    return 0;
}
