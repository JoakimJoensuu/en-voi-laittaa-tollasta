#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opening/file.h"
#include "parsing/parser.h"
#include "scanning/scanner.h"
#include "screening/screener.h"

void print_usage(char* program_name) {
    printf("usage: %s file\n", program_name);
    printf("    file: `.mpl` script file \n");
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

    char* source_code = file_read(arguments[1]);

    characters* screened = screen(source_code);

    printf("SCREENED LENGTH %d\n", screened->length);
    printf("%.*s\n", screened->length, screened->values);

    free(source_code);

    tokens* tokens = tokenize(screened);

    for (int i = 0; i < tokens->length; i++) {
        token t = tokens->values[i];
        printf("%s\t%.*s\n", token_type_string(t.type), t.length, t.value);
    }

    node* ast = parse(tokens);

    print_ast(ast, 0);
    printf("\n");

    return 0;
}
