
#ifndef SCANNING_SCANNER
#define SCANNING_SCANNER

#include "../screening/screener.h"
#include "tokens.h"

tokens*     tokenize(characters* text);
const char* token_type_string(token_type t);
#endif
