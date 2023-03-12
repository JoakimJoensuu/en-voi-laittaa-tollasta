#include "matchers.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

match delimiter_matcher(characters* text, int current_index) {
    int length = 0;

    if (text->values[current_index] == ' ') {
        length++;
    }

    return (match){
        .length     = length,
        .token_type = DELIMITER,
    };
}

bool is_separator(char character) {
    return character == ';' || character == '(' || character == ')';
}

match separator_matcher(characters* text, int current_index) {
    int length = 0;

    if (is_separator(text->values[current_index])) {
        length++;
    }

    return (match){
        .length     = length,
        .token_type = SEPARATOR,
    };
}

bool is_operator_character(char character) {
    return character == '+' || character == '-' || character == '*' ||
           character == '/' || character == '&' || character == '!' ||
           character == '=' || character == '<' || character == '>' ||
           character == ':' || character == '.';
}

match operator_matcher(characters* text, int current_index) {
    int length = 0;

    if (is_operator_character(text->values[current_index])) {
        length++;
    }

    if (text->values[current_index + length] == '=') {
        length++;
    }

    if (text->values[current_index] == '.' &&
        text->values[current_index + length] == '.') {
        length++;
    }

    return (match){
        .length     = length,
        .token_type = OPERATOR,
    };
}

bool is_keyword(char* start, int length) {
    if (length == sizeof("int") - 1 && memcmp(start, "int", length) == 0) {
        return true;
    }

    if (length == sizeof("var") - 1 && memcmp(start, "var", length) == 0) {
        return true;
    }
    if (length == sizeof("for") - 1 && memcmp(start, "for", length) == 0) {
        return true;
    }

    if (length == sizeof("if") - 1 && memcmp(start, "if", length) == 0) {
        return true;
    }

    if (length == sizeof("else") - 1 && memcmp(start, "else", length) == 0) {
        return true;
    }

    if (length == sizeof("in") - 1 && memcmp(start, "in", length) == 0) {
        return true;
    }

    if (length == sizeof("do") - 1 && memcmp(start, "do", length) == 0) {
        return true;
    }

    if (length == sizeof("string") - 1 &&
        memcmp(start, "string", length) == 0) {
        return true;
    }

    if (length == sizeof("bool") - 1 && memcmp(start, "bool", length) == 0) {
        return true;
    }

    if (length == sizeof("assert") - 1 &&
        memcmp(start, "assert", length) == 0) {
        return true;
    }

    if (length == sizeof("end") - 1 && memcmp(start, "end", length) == 0) {
        return true;
    }

    if (length == sizeof("read") - 1 && memcmp(start, "read", length) == 0) {
        return true;
    }

    if (length == sizeof("print") - 1 && memcmp(start, "print", length) == 0) {
        return true;
    }

    return false;
}

bool is_identifier_or_keyword_character(char character) {
    return (character >= 'a' && character <= 'z') ||
           (character >= 'A' && character <= 'Z') ||
           (character >= '0' && character <= '9');
}

match identifier_matcher(characters* text, int current_index) {
    if (text->values[current_index] >= '0' &&
        text->values[current_index] <= '9') {
        return (match){
            .length     = 0,
            .token_type = IDENTIFIER,
        };
    }

    int length = 0;

    while (current_index < text->length) {
        if (!is_identifier_or_keyword_character(
                text->values[current_index + length])) {
            break;
        }
        length++;
    }

    if (is_keyword(text->values + current_index, length)) {
        length = 0;
    }

    return (match){
        .length     = length,
        .token_type = IDENTIFIER,
    };
}

match keyword_matcher(characters* text, int current_index) {
    int length = 0;

    while (current_index + length < text->length) {
        if (!is_identifier_or_keyword_character(
                text->values[current_index + length])) {
            break;
        }
        length++;
    }

    if (!is_keyword(text->values + current_index, length)) {
        length = 0;
    }

    return (match){
        .length     = length,
        .token_type = KEYWORD,
    };
}

bool is_letter(char character) {
    return (character >= 'a' && character <= 'z') ||
           (character >= 'A' && character <= 'Z');
}

match literal_matcher(characters* text, int current_index) {
    int length = 0;

    if (text->values[current_index] == '"') {
        length++;
        while (current_index + length < text->length) {
            if (text->values[current_index + length] == '\\') {
                length += 2;
                continue;
            }

            if (text->values[current_index + length] == '"') {
                length++;
                return (match){
                    .length     = length,
                    .token_type = LITERAL,
                };
            }

            length++;
        }

        printf(
            "Error: Unterminated string literal starting at line %d, column "
            "%d!\n",
            text->positions[current_index].line,
            text->positions[current_index].column);
        exit(1);
    }

    if (text->values[current_index] >= '0' &&
        text->values[current_index] <= '9') {
        length++;
        while (current_index + length < text->length) {
            if (text->values[current_index + length] >= '0' &&
                text->values[current_index + length] <= '9') {
                length++;
                continue;
            }

            if (is_letter(text->values[current_index + length])) {
                printf(
                    "Error: Invalid integer constant starting at line %d, "
                    "column %d!\n",
                    text->positions[current_index].line,
                    text->positions[current_index].column);
                exit(1);
            }

            break;
        }

        return (match){
            .length     = length,
            .token_type = LITERAL,
        };
    }

    return (match){
        .length     = length,
        .token_type = LITERAL,
    };
}
