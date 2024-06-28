#include "parser.h" 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parser *parser_init() {
    Parser *p = (Parser *)malloc(sizeof(Parser));

    if (p == NULL) {
        return NULL;
    }

    p->p = 0;
    p->tokens_size = 10;
    p->token_count = 0;

    p->tokens = (Token *)malloc(sizeof(Token) * 10);
    if (p->tokens == NULL) {
        return NULL;
    }
    return p;
}

ParserCode parser_read_file(Parser *p, char *file) {
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

ParserCode parser_parse(Parser *p) {
    while (p->file[p->p] != '\0') {
        if (parser_parse_next(p) == FAILURE) {
            return FAILURE;
        }
    }

    if (p->token_count >= p->tokens_size) {
        p->token_count++;
        p->tokens = reallocarray(p->tokens, p->token_count, sizeof(Token));
        if (p->tokens == NULL) {
            return FAILURE;
        }
    }
    Token eof;
    eof.type = EOF;
    p->tokens[p->token_count] = eof;

    return PASS;
}

ParserCode parser_parse_next(Parser *p) {
    Token t;
    t.type = NONE;  
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
        case ' ':
            break;
        case '\n':
            break;
        default:
            if (isdigit(c)) {
                double n;
                if (!match_num(p, c, &n)) {
                    perror("Could not match number");
                }
                t.type = NUMBER;
                t.n = n;
            } else if (c == '"') {
                char *s = match_string(p);
                t.type = STRING;
                t.s = s;
                p->p++;
            } else if (c == 't' || c == 'f') {
                bool b;
                if (!match_bool(p, c, &b)) {
                    perror("Could not match boolean");
                }
                t.type = BOOL;
                t.b = b;
            }
            break;
    }

    if (t.type == NONE) {
        return PASS;
    }

    if (p->token_count >= p->tokens_size) {
        p->tokens_size *= 2;
        p->tokens = reallocarray(p->tokens, p->tokens_size, sizeof(Token));

        if (p->tokens == NULL) {
            return FAILURE;
        }
    }

    p->tokens[p->token_count++] = t;

    return PASS;
}

void parser_free(Parser *p) {
    int i = 0;
    while (p->tokens[i].type != EOF) {
        if (p->tokens[i++].type == STRING) {
            free(p->tokens[i].s);
        }
    }
    free(p->file);
    free(p->tokens);
    free(p);
}

// Whoever calls must free
char *parser_slice(Parser *p, int start, int end) {
    char *st = p->file + start;
    int l = sizeof(char) * (end - start);

    char *slice = (char *)malloc(l);
    if (slice == NULL) {
        return NULL;
    }

    memcpy(slice, st, l);
    return slice;
}

bool match_num(Parser *p, char f, double *n) {
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
                return false;
            }
        }

        if (p->file[p->p] == '.') {
            is_dec = 1;
        }

        buf[i++] = p->file[p->p++];
    }

    if (i >= size) {
        size++;
        if (!is_dec) {
            size += 2;
        }

        buf = realloc(buf, sizeof(char) * size);

        if (buf == NULL) {
            return false;
        }
    }
    if (!is_dec) {
        buf[i++] = '.';
        buf[i++] = '0';
    }
    buf[i] = '\0';
    *n = atof(buf);

    free(buf);
    return true;
}

// Whoever calls this must free the result
char *match_string(Parser *p) {
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

bool match_bool(Parser *p, char f, bool *b) {
    char *to_match = f == 't' ? "true" : "false";
    int len = strlen(to_match);

    char *slice = parser_slice(p, p->p - 1, p->p + len);
    if (slice == NULL) {
        return false;
    }

    if (strncmp(slice, to_match, len) == 0) { 
        *b = f == 't' ? true : false;
        free(slice);
        return true;
    }

    free(slice);
    return false;
}
