
#ifndef PARSING_AST
#define PARSING_AST

typedef enum {
    PROGRAM,
    STATEMENT,
    DECLARATION,
    ASSIGNMENT,
    FOR_LOOP,
    READ,
    PRINT,
    CONDITIONAL,
    INTEGER_LITERAL,
    STRING_LITERAL,
    EXPRESSION,
    VARIABLE,
    OPERATOR_NODE,
} node_type;

typedef struct node {
    node_type     type;
    int           children_count;
    int           children_size;
    struct node** children;
    void*         data;
    int           data_size;
} node;

typedef enum {
    BOOLEAN,
    INTEGER,
    STRING,
} variable_type;

typedef struct declaration_data {
    char*         identifier_name;
    int           name_length;
    variable_type type;
} declaration_data;

typedef struct assignment_data {
    char* identifier_name;
    int   name_length;
} assignment_data;

node* new_node(node_type type);

int   add_child(node* parent, node* child);
int   enlarge_child_list(node* target);
node* new_node(node_type type);
#endif
