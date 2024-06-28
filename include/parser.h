#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

typedef enum { FAILURE = 0, PASS = 1 } ParserCode;

typedef enum {
    BRACE_L = '{',
    BRACE_R = '}',
    BRACKET_L = '[',
    BRACKET_R = ']',
    COLON = ':',
    IDENTIFIER,
    COMMA = ',',
    NUMBER = 'n',
    STRING = '"',
    NONE = ' ',
    BOOL = 'T',
    EOF = '\0'
} TokenType;

typedef struct {
    TokenType type;
    char *s;
    double n;
    bool b;
} Token;

typedef struct {
    char *file;
    int p;

    Token *tokens;
    int tokens_size;
    int token_count;
} Parser;

extern Parser *parser_init();
extern ParserCode parser_read_file(Parser *p, char *file);
extern ParserCode parser_parse(Parser *p);
extern ParserCode parser_parse_next(Parser *p);
extern void parser_free(Parser *p);
extern char *parser_slice(Parser *p, int start, int end);
extern bool match_num(Parser *p, char f, double *n);
extern char *match_string(Parser *p);
extern bool match_bool(Parser *p, char f, bool *b);

#endif
