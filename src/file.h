#ifndef MINIPL_FILE
#define MINIPL_FILE

typedef unsigned char* minipl_contents;

int minipl_read(char*, minipl_contents*);

void minipl_free(minipl_contents);

#endif
