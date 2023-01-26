
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "file.h"

#define whitespace   \
    ' ' : case '\n': \
    case '\t':       \
    case '\v':       \
    case '\f':       \
    case '\r'

typedef struct {
    int row;
    int column;
    char data;
} character;

typedef struct state {
    struct state (*compute)(char*);
    char* character;
} state;

state empty_char(char* current) {
    state next = { .character = current + 1 };

    switch (*current) {
        case '\0':
            next.compute = NULL;
            break;
        case whitespace:
            next.compute = &empty_char;
            break;
        default:
            printf("UNKNOWN CHARACTER '%c'!\n", *current);
            next.compute = &empty_char;

            break;
    }

    return next;
}

int minipl_scan(minipl_contents contents) {
    int row = 0;
    int column = 0;

    char* string = "ABCDEFGHIJLKMN";

    state current = {
        .compute = &empty_char,
        .character = contents.data,
    };

    while (current.compute != NULL) {
        state next = current.compute(current.character);
        current = next;
    }

    return 0;
}
