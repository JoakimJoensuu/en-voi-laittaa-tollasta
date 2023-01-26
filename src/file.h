#ifndef MINIPL_FILE
#define MINIPL_FILE

typedef struct minipl_contents {
    char* data;
} minipl_contents;

int minipl_read(char* filename, minipl_contents* target);

int minipl_free(minipl_contents contents);

#endif
