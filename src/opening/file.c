#include "file.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* file_read(char* filename) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Unable to open file %s (%s)\n", filename, strerror(errno));
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    printf("Opened file %s of size %d bytes\n", filename, file_size);

    int   string_size              = file_size + 1;
    char* file_contents            = malloc((string_size) * sizeof(char));
    file_contents[string_size - 1] = '\0';

    int read_bytes = fread(file_contents, sizeof(char), file_size, file);

    if (read_bytes != file_size) {
        printf("Something went wrong while reading the file.");
        exit(1);
    }

    fclose(file);

    return file_contents;
}
