#ifndef SCREENING_CHARACTERS
#define SCREENING_CHARACTERS

typedef struct position {
    int line;
    int column;
} position;

typedef struct characters {
    char*     values;
    position* positions;
    int       length;
    int       size;
} characters;

characters* new_characters();
int         append_char(characters* target, char appendable, position pos);

#endif
