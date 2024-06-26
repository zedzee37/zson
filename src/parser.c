#include "parser.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Parser *parser_init() {
    struct Parser *p = (struct Parser *)malloc(sizeof(struct Parser));

    if (p == NULL) {
        return NULL;
    }

    p->p = 0;
    p->tokens_size = 10;
    p->token_count = 0;

    p->tokens = (struct Token *)malloc(sizeof(struct Token) * 10);
    if (p->tokens == NULL) {
        return NULL;
    }
    return p;
}

enum ParserCode parser_read_file(struct Parser *p, char *file) {
    FILE *fp = fopen(file, "r");

    if (fp == NULL) {
        return FAILURE;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    p->file = (char *)malloc(size + sizeof(char));
    if (p->file == NULL) {
        return FAILURE;
    }

    p->file[size / sizeof(char)] = '\0';
    fread(p->file, sizeof(char), size, fp);

    fclose(fp);
    return PASS;
}

enum ParserCode parser_parse(struct Parser *p) {
    while (p->file[p->p] != '\0') {
        if (parser_parse_next(p) == FAILURE) {
            return FAILURE;
        }
    }

    if (p->token_count >= p->tokens_size) {
        p->tokens = realloc(p->tokens, sizeof(struct Token) * ++p->token_count);
        if (p->tokens == NULL) {
            return FAILURE;
        }
    }
    struct Token eof;
    eof.type = EOF;
    p->tokens[p->token_count] = eof;

    return PASS;
}

enum ParserCode parser_parse_next(struct Parser *p) {
    struct Token t;
    char c = p->file[p->p++];

    switch (c) {
        case BRACE_L:
            t.type = BRACE_L;
            break;
        case BRACE_R:
            t.type = BRACE_R;
            break;
        case BRACKET_L:
            t.type = BRACKET_L;
            break;
        case BRACKET_R:
            t.type = BRACKET_R;
            break;
        case COLON:
            t.type = COLON;
            break;
        case COMMA:
            t.type = COMMA;
            break;
        default:
            if (isdigit(c)) {
                double n;
                match_num(p, c, &n);
                t.type = NUMBER;
                t.val = &n;
            } else if (c == '"') {
                char *s = match_string(p);
                t.type = STRING;
                t.val = s;
                p->p++;
            }
            break;
    }

    if (p->token_count >= p->tokens_size) {
        p->tokens_size *= 2;
        p->tokens = realloc(p->tokens, sizeof(struct Token) * p->tokens_size);

        if (p->tokens == NULL) {
            return FAILURE;
        }
    }
    p->tokens[p->token_count++] = t;

    return PASS;
}

void parser_free(struct Parser *p) {
    free(p->file);
    free(p->tokens);
    free(p);
}

char *parser_slice(struct Parser *p, int start, int end) {
    char *st = p->file + start;
    int l = sizeof(char) * (end - start);
    char *slice = (char *)malloc(l);
    memcpy(slice, st, l);
    return slice;
}

void match_num(struct Parser *p, char f, double *n) {
    int i = 0;

    size_t size = 10;
    char *buf = (char *)malloc(sizeof(char) * size);
    buf[i++] = f;

    int is_dec = 0;

    while (isdigit(p->file[p->p]) || (!is_dec && p->file[p->p] == '.')) {
        if (i >= size) {
            size *= 2;
            buf = realloc(buf, sizeof(char) * size);

            if (buf == NULL) {
                return;
            }
        }

        if (p->file[p->p] == '.') {
            is_dec = 1;
        }

        buf[i++] = p->file[p->p++];
    }

    if (i >= size) {
        size++;
        buf = realloc(buf, sizeof(char) * size);

        if (buf == NULL) {
            return;
        }
    }
    buf[i] = '\0';

    *n = atof(buf);
    free(buf);
}

// Whoever calls this must free the result
char *match_string(struct Parser *p) {
    int i = 0;

    size_t size = 10;
    char *buf = (char *)malloc(sizeof(char) * size);

    while (p->file[p->p] != '"') {
        if (i >= size) {
            size *= 2;
            buf = realloc(buf, sizeof(char) * size);

            if (buf == NULL) {
                return NULL;
            }
        }
        buf[i++] = p->file[p->p++];
    }

    if (i >= size) {
        size++;
        buf = realloc(buf, sizeof(char) * size);

        if (buf == NULL) {
            return NULL;
        }
    }
    buf[i] = '\0';

    return buf;
}
