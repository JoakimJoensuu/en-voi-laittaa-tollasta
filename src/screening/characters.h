#ifndef SCREENING_CHARACTERS
#define SCREENING_CHARACTERS

typedef struct position {
    int line;
    int column;
} position;

typedef struct character {
    unsigned char     value;
    position          position;
    struct character* next;
} character;

character* new_characters();

#endif
