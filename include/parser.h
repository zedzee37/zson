#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

enum ParserCode { FAILURE = 0, PASS = 1 };

enum TokenType {
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
};

struct Token {
    enum TokenType type;
    char *s;
    double n;
    bool b;
};

struct Parser {
    char *file;
    int p;

    struct Token *tokens;
    int tokens_size;
    int token_count;
};

extern struct Parser *parser_init();
extern enum ParserCode parser_read_file(struct Parser *p, char *file);
extern enum ParserCode parser_parse(struct Parser *p);
extern enum ParserCode parser_parse_next(struct Parser *p);
extern void parser_free(struct Parser *p);
extern char *parser_slice(struct Parser *p, int start, int end);
extern bool match_num(struct Parser *p, char f, double *n);
extern char *match_string(struct Parser *p);
extern bool match_bool(struct Parser *p, char f, bool *b);

#endif
