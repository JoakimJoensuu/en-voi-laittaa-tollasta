#ifndef PARSING_PARSER
#define PARSING_PARSER

#include "../scanning/scanner.h"
#include "ast.h"

void print_ast(node* ast, int level);

node* parse(tokens* tokens);

#endif
