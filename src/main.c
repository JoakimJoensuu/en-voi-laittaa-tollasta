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

    return 0;
}
