
#ifndef SCANNING_SCANNER
#define SCANNING_SCANNER

#include "../screening/screener.h"
#include "tokens.h"

tokens*     tokenize(characters* text);
const char* type_string(type t);
#endif
