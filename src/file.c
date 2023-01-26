#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "file.h"

int minipl_read(char *filename, char **target)
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("Unable to open file %s (%s)\n", filename, strerror(errno));
        return 1;
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    printf("Opened file %s of size %d bytes\n", filename, file_size);

    *target = malloc((file_size + 1) * sizeof(char));
    (*target)[file_size] = '\0';
    int read_bytes = fread(*target, sizeof(char), file_size, file);

    if (read_bytes != file_size)
    {
        printf("Something went wrong while reading the file.");
        return 1;
    }

    fclose(file);

    return 0;
}

int minipl_free(char *contents)
{
    free(contents);
}
