#include "scanner.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matchers.h"

matcher* matchers[] = {
    &identifier_matcher, &literal_matcher, &operator_matcher,
    &separator_matcher,  &keyword_matcher, &delimiter_matcher,
};

const char* type_string(type t) {
    switch (t) {
        case UNKNOWN:
            return "UNKNOWN";
        case IDENTIFIER:
            return "IDENTIFIER";
        case LITERAL:
            return "LITERAL";
        case OPERATOR:
            return "OPERATOR";
        case SEPARATOR:
            return "SEPARATOR";
        case KEYWORD:
            return "KEYWORD";
        case DELIMITER:
            return "DELIMITER";
        default:
            return "UNKNOWN TYPE";
    }
}

tokens* tokenize(characters* text) {
    int current_index = 0;

    tokens* tokens = new_tokens();

    while (current_index != text->length) {
        match longest = {
            .length = 0,
            .type   = UNKNOWN,
        };

        for (unsigned int i = 0; i < sizeof(matchers) / sizeof(matcher*); i++) {
            match new = matchers[i](text, current_index);

            if (new.length > longest.length) {
                if (longest.length != 0) {
                    printf("MULTIPLE MATCHING TOKENS!\n");
                    exit(1);
                }

                longest = new;
            }
        }

        if (longest.type == UNKNOWN) {
            printf("Could not tokenize\n");
            printf("!%.*s!", text->length - current_index,
                   text->values + current_index);
            exit(1);
        }

        if (longest.type == DELIMITER) {
            current_index += longest.length;
            continue;
        }

        char* value = malloc(longest.length);
        memcpy(value, text->values + current_index,
               longest.length * sizeof(char));

        token new = {
            .type   = longest.type,
            .value  = value,
            .length = longest.length,
            .line   = text->positions[current_index].line,
            .column = text->positions[current_index].column,
        };

        append_tokens(tokens, new);

        current_index += longest.length;
    }

    return tokens;
}
