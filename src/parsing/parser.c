#include "parser.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

typedef struct {
    node* child;
    int   tokens_consumed;
} match_result;

typedef match_result matcher(tokens* tokens, int index);

match_result expression(tokens* tokens, int index);

bool is_type_and_value(token token, token_type type, const char* value) {
    if (token.type != type) {
        return false;
    }

    int value_length = strlen(value);
    if (token.length != value_length) {
        return false;
    }

    if (memcmp(token.value, value, token.length) != 0) {
        return false;
    }

    return true;
};

bool is_type_and_one_of_values(token token, token_type type,
                               const char** values, int values_count) {
    for (int i = 0; i < values_count; i++) {
        if (is_type_and_value(token, type, values[i])) {
            return true;
        }
    }

    return false;
}

bool has_value(token token, const char* value) {
    int value_length = strlen(value);
    if (token.length != value_length) {
        return false;
    }

    if (memcmp(token.value, value, token.length) != 0) {
        return false;
    }

    return true;
};

match_result variable(tokens* tokens, int index) {
    int   original_index = index;
    node* nodes          = NULL;
    token current_token  = tokens->values[index++];

    if (current_token.type == IDENTIFIER) {
        nodes = new_node(VARIABLE);

        nodes->data = malloc(current_token.length);
        memcpy(nodes->data, current_token.value, current_token.length);
        nodes->data_size = current_token.length;
    } else {
        printf("Expected identifier, got %s \"%.*s\"!\n",
               token_type_string(current_token.type), current_token.length,
               current_token.value);
    }

    return (match_result){.child           = nodes,
                          .tokens_consumed = index - original_index};
}

match_result operand(tokens* tokens, int index) {
    int   original_index = index;
    node* nodes          = NULL;

    if (tokens->values[index].type == LITERAL) {
        if (*tokens->values[index].value == '"') {
            printf("Parsing string literals not yet implemented!\n");
            exit(1);
        } else {
            char* buffer =
                calloc(tokens->values[index].length + 1, sizeof(*buffer));

            memcpy(buffer, tokens->values[index].value,
                   tokens->values[index].length);

            nodes = new_node(INTEGER_LITERAL);

            int value   = atoi(buffer);
            nodes->data = malloc(sizeof(value));
            memcpy(nodes->data, &value, sizeof(value));
            nodes->data_size = sizeof(value);

            free(buffer);
        }
        index++;
        // int, bool or string literal
    } else if (is_type_and_value(tokens->values[index], SEPARATOR, "(")) {
        index++;
        match_result expression_result = expression(tokens, index);

        nodes = expression_result.child;
        index += expression_result.tokens_consumed;

        token current_token = tokens->values[index++];
        if (!is_type_and_value(current_token, SEPARATOR, ")")) {
            printf(
                "Expected closing bracket \")\" at line %d, column %d, got %s "
                "\"%.*s\"\n",
                current_token.line, current_token.column,
                token_type_string(current_token.type), current_token.length,
                current_token.value);
            exit(1);
        }

    } else if (tokens->values[index].type == IDENTIFIER) {
        match_result variable_result = variable(tokens, index);

        nodes = variable_result.child;
        index += variable_result.tokens_consumed;
    } else {
        token current_token = tokens->values[index];
        printf(
            "Expected identifier, literal or opening bracket at line %d, "
            "column "
            "%d, got %s \"%.*s\"\n",
            current_token.line, current_token.column,
            token_type_string(current_token.type), current_token.length,
            current_token.value);
        exit(1);
    }

    return (match_result){.child           = nodes,
                          .tokens_consumed = index - original_index};
}

match_result expression(tokens* tokens, int index) {
    int original_index = index;

    if (is_type_and_value(tokens->values[index], OPERATOR, "!")) {
        printf("Parsing for unary operand \"!\" not yet implemented.");
        exit(1);
    }

    match_result first_operand_result = operand(tokens, index);
    node*        first_operand        = first_operand_result.child;

    index += first_operand_result.tokens_consumed;

    if (tokens->values[index].type != OPERATOR) {
        return (match_result){.child           = first_operand,
                              .tokens_consumed = index - original_index};
    }

    token current_token = tokens->values[index++];

    node* operator= new_node(OPERATOR_NODE);
          operator->data = malloc(current_token.length);
    memcpy(operator->data, current_token.value, current_token.length);
    operator->data_size = current_token.length;

    add_child(operator, first_operand);

    match_result second_operand_result = operand(tokens, index);
    node*        second_operand        = second_operand_result.child;
    index += second_operand_result.tokens_consumed;

    add_child(operator, second_operand);

    return (match_result){.child           = operator,
                          .tokens_consumed = index - original_index };
};

match_result declaration(tokens* tokens, int index) {
    int   original_index = index;
    token current_token  = tokens->values[index++];

    if (current_token.type != IDENTIFIER) {
        printf(
            "Expected identifier name at line %d, column %d, got %s \"%.*s\"\n",
            current_token.line, current_token.column,
            token_type_string(current_token.type), current_token.length,
            current_token.value);
        exit(1);
    }

    int   name_length = current_token.length;
    char* name        = malloc(name_length);
    memcpy(name, current_token.value, name_length);

    current_token = tokens->values[index++];
    if (!is_type_and_value(current_token, OPERATOR, ":")) {
        printf(
            "Expected operator \":\" at line %d, column %d, got %s \"%.*s\"\n",
            current_token.line, current_token.column,
            token_type_string(current_token.type), current_token.length,
            current_token.value);
        exit(1);
    }

    node* statement = new_node(STATEMENT);

    current_token = tokens->values[index++];
    variable_type var_type;
    if (is_type_and_value(current_token, KEYWORD, "int")) {
        var_type = INTEGER;
    } else if (is_type_and_value(current_token, KEYWORD, "string")) {
        var_type = STRING;
    } else if (is_type_and_value(current_token, KEYWORD, "bool")) {
        var_type = BOOLEAN;
    } else {
        printf(
            "Expected variable type at line %d, column %d, got %s \"%.*s\"\n",
            current_token.line, current_token.column,
            token_type_string(current_token.type), current_token.length,
            current_token.value);
        exit(1);
    }

    node* declaration = new_node(DECLARATION);

    declaration_data data = {
        .type            = var_type,
        .identifier_name = name,
        .name_length     = name_length,
    };

    declaration->data = malloc(sizeof(data));
    memcpy(declaration->data, &data, sizeof(data));
    add_child(statement, declaration);

    if (is_type_and_value(tokens->values[index], OPERATOR, ":=")) {
        index++;

        node* assignment = new_node(ASSIGNMENT);

        char* assignment_name = malloc(data.name_length);
        memcpy(assignment_name, data.identifier_name, data.name_length);
        assignment_data assignment_data = {
            .identifier_name = assignment_name,
            .name_length     = data.name_length,
        };

        assignment->data = malloc(sizeof(assignment_data));
        memcpy(assignment->data, &assignment_data, sizeof(assignment_data));

        match_result expression_result = expression(tokens, index);

        index += expression_result.tokens_consumed;
        add_child(assignment, expression_result.child);

        add_child(statement, assignment);
    }

    return (match_result){.child           = statement,
                          .tokens_consumed = index - original_index};
};

match_result print(tokens* tokens, int index) {
    int original_index = index;

    node* print = new_node(PRINT);

    match_result expression_result = expression(tokens, index);
    index += expression_result.tokens_consumed;
    add_child(print, expression_result.child);

    node* statement = new_node(STATEMENT);
    add_child(statement, print);

    return (match_result){.child           = statement,
                          .tokens_consumed = index - original_index};
};

match_result statement(tokens* tokens, int index) {
    int          original_index = index;
    node*        nodes          = NULL;
    token        current_token  = tokens->values[index++];
    match_result result         = {0};

    if (has_value(current_token, "var")) {
        result = declaration(tokens, index);

    } else if (current_token.type == IDENTIFIER) {
        printf("Parsing assignments is not yet implemented\n");
        exit(1);
    } else if (has_value(current_token, "for")) {
        printf("Parsing for loops is not yet implemented.\n");
        exit(1);
    } else if (has_value(current_token, "read")) {
        printf("Parsing read is not yet implemented.\n");
        exit(1);
    } else if (has_value(current_token, "print")) {
        result = print(tokens, index);

    } else if (has_value(current_token, "if")) {
        printf("Parsing if statements is not yet implemented.\n");
        exit(1);
    }

    index += result.tokens_consumed;
    nodes = result.child;

    if (nodes == NULL) {
        printf(
            "Expected keyword or identifier at line %d, column %d, got %s "
            "\"%.*s\"\n",
            current_token.line, current_token.column,
            token_type_string(current_token.type), current_token.length,
            current_token.value);
        exit(1);
    }

    current_token = tokens->values[index++];
    if (!is_type_and_value(current_token, SEPARATOR, ";")) {
        printf(
            "Expected separator \";\" at line %d, column %d, got %s \"%.*s\"\n",
            current_token.line, current_token.column,
            token_type_string(current_token.type), current_token.length,
            current_token.value);
        exit(1);
    }

    return (match_result){.tokens_consumed = index - original_index,
                          .child           = nodes};
}

const char* node_type_string(node_type t) {
    switch (t) {
        case PROGRAM:
            return "PROGRAM";
        case STATEMENT:
            return "STATEMENT";
        case DECLARATION:
            return "DECLARATION";
        case ASSIGNMENT:
            return "ASSIGNMENT";
        case FOR_LOOP:
            return "FOR_LOOP";
        case READ:
            return "READ";
        case PRINT:
            return "PRINT";
        case CONDITIONAL:
            return "CONDITIONAL";
        case INTEGER_LITERAL:
            return "INTEGER_LITERAL";
        case STRING_LITERAL:
            return "STRING_LITERAL";
        case EXPRESSION:
            return "EXPRESSION";
        case VARIABLE:
            return "VARIABLE";
        case OPERATOR_NODE:
            return "OPERATOR_NODE";
        default:
            printf("Node type to string not implemented (enum %d)\n", t);
            exit(1);
            return NULL;
    }
}

char* variable_type_string(variable_type t) {
    switch (t) {
        case BOOLEAN:
            return "boolean";
        case INTEGER:
            return "integer";
        case STRING:
            return "string";
        default:
            printf("Variable type to string not implemented (enum %d)\n", t);
            exit(1);
            return NULL;
    }
}

void print_ast(node* ast, int level) {
    printf("%s", node_type_string(ast->type));

    if (ast->type == DECLARATION) {
        declaration_data d = *(declaration_data*)ast->data;
        printf(" (%.*s %s)", d.name_length, d.identifier_name,
               variable_type_string(d.type));
    } else if (ast->type == ASSIGNMENT) {
        assignment_data d = *(assignment_data*)ast->data;
        printf(" (%.*s)", d.name_length, d.identifier_name);
    } else if (ast->type == INTEGER_LITERAL) {
        int d = *(int*)ast->data;
        printf(" (%d)", d);
    } else if (ast->type == VARIABLE) {
        char* d      = (char*)ast->data;
        int   length = ast->data_size / sizeof(*d);
        printf(" (%.*s)", length, d);
    } else if (ast->type == OPERATOR_NODE) {
        char* d      = (char*)ast->data;
        int   length = ast->data_size / sizeof(*d);
        printf(" (%.*s)", length, d);
    } else if (ast->type == PRINT || ast->type == STATEMENT ||
               ast->type == PROGRAM) {
        printf(" ()");
    } else {
        printf(" (noimplementation)");
    }

    if (ast->children_count > 0) {
        printf(" {\n");
        for (int i = 0; i < ast->children_count; i++) {
            for (int j = 0; j <= level; j++) {
                printf("\t");
            }

            print_ast(ast->children[i], level + 1);
            printf("\n");
        }
        for (int k = 0; k < level; k++) {
            printf("\t");
        }

        printf("}");
    }
}

node* parse(tokens* tokens) {
    node* ast = new_node(PROGRAM);

    for (int current_index = 0; current_index < tokens->length;) {
        match_result result = statement(tokens, current_index);

        add_child(ast, result.child);
        current_index += result.tokens_consumed;
    }

    return ast;
}
