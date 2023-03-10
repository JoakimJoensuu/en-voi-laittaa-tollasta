
#ifndef SCANNING_MATCHERS
#define SCANNING_MATCHERS

#include "../screening/characters.h"
#include "scanner.h"

typedef struct {
    token_type token_type;
    int        length;
} match;

typedef match matcher(characters* text, int current_index);

match keyword_matcher(characters* text, int current_index);
match separator_matcher(characters* text, int current_index);
match operator_matcher(characters* text, int current_index);
match literal_matcher(characters* text, int current_index);
match identifier_matcher(characters* text, int current_index);
match delimiter_matcher(characters* text, int current_index);

#endif
