#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
gcc -g miscellaneous/string_scanning.c -o string_scanning \
&& valgrind --track-origins=yes --leak-check=full \
--show-leak-kinds=all ./string_scanning
*/

typedef struct char_list {
    char* data;
    int length;
    int size;
}* char_list;

char_list init_char_list()
{
    char_list new = malloc(sizeof(struct char_list));
    new->data = malloc(1);
    new->length = 0;
    new->size = 1;

    return new;
}

void free_char_list(char_list new)
{
    free(new->data);
    free(new);
}

void enlarge_char_list(char_list target)
{
    int new_size = target->size * 2;
    char* new_data = malloc(new_size);
    memcpy(new_data, target->data, target->size);
    free(target->data);
    target->data = new_data;

    target->size *= 2;
}

void append_char(char_list target, char appendable)
{
    if (target->size - target->length * sizeof(char)
        < sizeof(char)) {
        enlarge_char_list(target);
    }

    target->data[target->length] = appendable;
    target->length++;
}

void append_n_char(char_list target, const char* src,
                   int count)
{
    if (count == 0) {
        return;
    }

    while (target->size - target->length * sizeof(char)
           < count * sizeof(char)) {
        enlarge_char_list(target);
    }

    memcpy(target->data + target->length, src,
           count * sizeof(char));

    target->length += count;
}

void append_string(char_list target, char* string)
{
    int length = strlen(string);

    while (target->size - target->length * sizeof(char)
           < length * sizeof(char)) {
        enlarge_char_list(target);
    }

    memcpy(&target->data[target->length], string,
           length * sizeof(char));

    target->length += length;
}

int scan_string(const char* start_of_a_string,
                char** destination)
{
    char_list result = init_char_list();

    for (const char *start = start_of_a_string,
                    *current = start_of_a_string;
         ; current++) {
        if (*current == '\0' || *current == '\n') {
            *destination = NULL;
            free_char_list(result);
            return 1;
        }

        if (*current == '\\') {
            append_n_char(result, start, current - start);
            current++;
            switch (*current) {
                case 'n':
                    append_char(result, '\n');
                    break;
                case 't':
                    append_char(result, '\t');
                    break;
                case 'v':
                    append_char(result, '\v');
                    break;
                case 'f':
                    append_char(result, '\f');
                    break;
                case 'r':
                    append_char(result, '\r');
                    break;
                case '\\':
                    append_char(result, '\\');
                    break;
                case '"':
                    append_char(result, '"');
                    break;
                default:
                    printf(
                        "Escaped something that is not "
                        "implemented yet %c\n",
                        *current);
                    break;
            }

            start = current + 1;
            continue;
        }

        if (*current == '"') {
            append_n_char(result, start, current - start);
            break;
        }
    };

    *destination = calloc(result->length + 1, sizeof(char));
    memcpy(*destination, result->data, result->length);
    free_char_list(result);

    return 0;
}

char* find_beginning_of_a_string(char* text)
{
    while (*text != '"') {
        if (*text == '\0') {
            return NULL;
        }
        text++;
    }

    return text + 1;
}

char* find_string(char* text)
{
    char* start_of_a_string =
        find_beginning_of_a_string(text);

    if (start_of_a_string == NULL) {
        return NULL;
    }

    char* result;
    if (scan_string(start_of_a_string, &result) != 0) {
        return NULL;
    }

    return result;
}

int main()
{
    char* texts[] = {
        "var text=\"Some text\\n\\tand backslash \\\\ "
        "\\\"some\\\" more\";var int=1",
        "var text=\"Some text\\n\\tand backslash \\\\ "
        "\\\"some\\\" more;var int=1"
    };

    for (char** text = texts;
         text - texts < sizeof(texts) / sizeof(*texts);
         text++) {
        printf("TEXT:\n%s\n\n", *text);
        char* resulting_string = find_string(*text);

        if (resulting_string == NULL) {
            printf("Did not find string.\n");
            continue;
        }

        printf(
            "Found following "
            "string:\n\"\"\"\n%s\n\"\"\"\n\n",
            resulting_string);

        free(resulting_string);
    }

    return 0;
}
