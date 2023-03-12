#ifndef SCANNING_TOKENS
#define SCANNING_TOKENS

typedef enum {
    IDENTIFIER,
    LITERAL,
    UNKNOWN,
    OPERATOR,
    SEPARATOR,
    KEYWORD,
    DELIMITER
} token_type;

typedef struct {
    token_type  type;
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
