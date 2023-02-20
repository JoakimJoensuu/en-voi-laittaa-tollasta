#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "screening/screener.h"

#define SUCCESS 0

void print_usage(char* program_name) {
    printf("usage: %s file\n", program_name);
    printf("    file: program read from .mpl -file \n");
    printf("\n");
    printf("example: %s hello_world.mpl\n", program_name);
}

bool strings_are_same(char* str1, char* str2) {
    if (!strcmp(str1, str2)) {
        return true;
    }

    return false;
}

int main(int argument_count, char* arguments[]) {
    if (argument_count != 2 || strings_are_same("--help", arguments[1]) ||
        strings_are_same("-h", arguments[1])) {
        print_usage(arguments[0]);
        return 1;
    }

    minipl_contents contents = {0};

    if (minipl_read(arguments[1], &contents) != SUCCESS) {
        exit(1);
    }

    character* screened = minipl_screen(contents);

    printf("PRINT SCANNED\n");
    printf("\"\"\"\n!");
    character* next = screened->next;
    free(screened);
    while (next != NULL) {
        printf("%c", next->value);
        character* previous = next;
        next                = previous->next;
        free(previous);
    }
    printf("!\n\"\"\"\n\n");

    minipl_free(contents);

    return 0;
}
