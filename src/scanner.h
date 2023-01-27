#ifndef MINIPL_SCANNER
#define MINIPL_SCANNER

#define whitespace   \
    ' ' : case '\n': \
    case '\t':       \
    case '\v':       \
    case '\f':       \
    case '\r'

#define operator_or_bracket_char \
    '(' : case ')':              \
    case '*':                    \
    case '-':                    \
    case '+':                    \
    case '=':                    \
    case '!':                    \
    case ';':                    \
    case '&':                    \
    case ':'

typedef struct state_context {
    char* data;
    int line;
    int column;
} state_context;

typedef struct state {
    struct state (*compute)(state_context);
    state_context context;
} state;

int minipl_scan(struct minipl_contents contents);

state one_line_comment(state_context current);

state asterisk_in_multiline_comment(state_context current);

state multiline_comment(state_context current);

state first_forward_slash(state_context current);

state normal_character(state_context current);

state quotation(state_context current);

state empty_char(state_context current);

state operator_or_bracket(state_context current);

#endif
