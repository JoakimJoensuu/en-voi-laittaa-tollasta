#ifndef SCANNING_TOKENS
#define SCANNING_TOKENS

typedef enum {
    IDENTIFIER,
    LITERAL,
    UNKNOWN,
    EMPTY,
    OPERATOR,
    SEPARATOR,
    KEYWORD,
} type;

typedef struct {
    type  type;
    char* value;
    int   length;
    int   line;
    int   column;
} token;

typedef struct tokens {
    token* values;
    int    length;
    int    size;
} tokens;

tokens* new_tokens();
int     append_tokens(tokens* target, token appendable);

#endif
