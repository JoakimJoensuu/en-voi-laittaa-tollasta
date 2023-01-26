#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void print_usage(char *program_name)
{
    printf("usage: %s file\n", program_name);
    printf("    file: program read from .mpl -file \n");
    printf("\n");
    printf("example: %s hello_world.mpl\n", program_name);
}

bool strings_are_same(char *str1, char *str2)
{
    if (!strcmp(str1, str2))
    {
        return true;
    }

    return false;
}

int main(int argument_count, char *arguments[])
{
    if (argument_count != 2 || strings_are_same("--help", arguments[1]) || strings_are_same("-h", arguments[1]))
    {
        print_usage(arguments[0]);
        return 1;
    }

    FILE *file = fopen(arguments[1], "r");

    if (file == NULL)
    {
        printf("Unable to open file %s (%s)\n", arguments[1], strerror(errno));
    }

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    rewind(file);
    printf("Opened file %s of size %d bytes\n", arguments[1], file_size);

    char file_contents[file_size + 1];
    file_contents[file_size] = '\0';
    fread(file_contents, sizeof(char), file_size, file);
    printf("%s\n", file_contents);

    return 0;
}
