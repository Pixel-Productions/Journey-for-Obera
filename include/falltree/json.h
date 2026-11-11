#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"
#include "dict.h"
#include "error.h"

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum JSON_TOKEN_TYPE {
    LBRACE,
    RBRACE,
    LBRACK,
    RBRACK,
    COLON,
    COMMA,
    NUMBER,
    STRING,
    BOOL
};

typedef struct {
    enum JSON_TOKEN_TYPE type;

    union {
        int intval;
        char *strval;
        bool boolval;
    } val;
    size_t strval_len;
} json_token_t;

typedef struct {
    json_token_t *tokens;
    size_t tokenc;
} json_token_table_t;

static bool is_int(char val) {
    return val >= '0' && val <= '9';
}

static bool is_alpha(char val) {
    return (val >= 'A' && val <= 'Z') || (val >= 'a' && val <= 'z');
}

static void strcat_rs(char *dest, char *src) {
    dest = (char*)realloc(dest, strlen(dest) + strlen(src) - 1);
    strcat(dest, src);
}

void json_free_token_table(json_token_table_t tab) {
    for (int i = 0; i < tab.tokenc; i++) {
        json_token_t tok = tab.tokens[i];

        if (tok.type == STRING)
            free(tok.val.strval);
    }

    free(tab.tokens);
}

static json_token_table_t json_lex(char *pth) {
    FILE *file = fopen(pth, "r");

    fseek(file, 0, SEEK_END);
    int len = ftell(file);
    rewind(file);

    unsigned char json[len+1];
    fread(json, 1, len, file);
    json[len] = 0;


    json_token_table_t tab;

    tab.tokens = (json_token_t*)malloc(0);
    tab.tokenc = 0;

    for (int i = 0; i < len; i++) {
        json_token_t tok;

        switch (json[i]) {
            case '{': {
                tok.type = LBRACE;
                break;
            }
            case '}': {
                tok.type = RBRACE;
                break;
            }
            case '[': {
                tok.type = LBRACK;
                break;
            }
            case ']': {
                tok.type = RBRACK;
                break;
            }
            case ':': {
                tok.type = COLON;
                break;
            }
            case ',': {
                tok.type = COMMA;
                break;
            }
            default: {
                if (is_int(json[i])) {
                    tok.type = NUMBER;

                    int val;

                    while (is_int(json[i]))
                        val = val * 10 + json[i++];

                    tok.val.intval = val;
                } else if (json[i] == '"') {
                    tok.type = STRING;

                    char *val = (char*)malloc(0);

                    while (is_alpha(json[i])) {
                        char chr[2] = { (char)json[++i], 0 };
                        strcat_rs(val, chr);
                    }

                    if (json[i] != '"') {
                        error("Unexpected token in file %s: %c", pth, json[i]);

                        json_free_token_table(tab);
                        exit(1);
                    }

                    tok.val.strval = val;
                    i++;
                } else if (is_alpha(json[i])) {
                    tok.type = BOOL;

                    char *val = (char*)malloc(0);

                    while (is_alpha(json[i])) {
                        char chr[2] = { (char)json[i++], 0 };
                        strcat_rs(val, chr);
                    }

                    if (strcmp(val, "true") == 0)
                        tok.val.boolval = true;
                    else if (strcmp(val, "false") == 0)
                        tok.val.boolval = false;
                    else {
                        error("Unexpected token in file %s: %c", pth, json[i]);

                        json_free_token_table(tab);
                        exit(1);
                    }
                } else {
                    error("Nah wtf is you doin ToT in file %s: %c", pth, json[i]);

                    json_free_token_table(tab);
                    exit(1);
                }
            }
        }

        tab.tokenc++;
        tab.tokens = (json_token_t*)realloc(tab.tokens, tab.tokenc*sizeof(json_token_t));

        tab.tokens[tab.tokenc-1] = tok;
    }

    return tab;
}

static int parse_json_token(dict_t dpar, list_t lpar, char *key, json_token_table_t tab, int i) {
    json_token_t tok = tab.tokens[i];

    switch (tok.type) {
        case LBRACE: {
            if (lpar == NULL && key == NULL) {
                error("Expected key");
                return -1;
            }
            
            dict_t dict = dict_init();

            while (tab.tokens[++i].type != RBRACE)
                parse_json_token(dict, NULL, key, tab, i);

            if (lpar == NULL)
                dict_add_elem(dpar, key, dict);
            else if (dpar == NULL)
                list_add_elem(lpar, dict);
            else {
                error("??????????");
                return -1;
            }

            i++;
            break;
        }
        case RBRACE: {
            error("Unexpected '}'");
            return -1;
        }
        case LBRACK: {
            if (lpar == NULL && key == NULL) {
                error("Expected key");
                return -1;
            }
            
            list_t list = list_init();

            while (tab.tokens[++i].type != RBRACK)
                parse_json_token(NULL, list, key, tab, i);
            
            if (lpar == NULL)
                dict_add_elem(dpar, key, list);
            else if (dpar == NULL)
                list_add_elem(lpar, list);
            else {
                error("??????????");
                return -1;
            }

            i++;
            break;
        }
        case RBRACK: {
            error("Unexpected ']'");
            return -1;
        }
        case COLON: {
            json_token_t name = tab.tokens[i - 1];

            if (name.type != STRING) {
                error("Invalid key");
                return -1;
            }

            key = name.val.strval;
            break;
        }
        case NUMBER: {
            int *val = (int*)malloc(sizeof(int));

            *val = tok.val.intval;
            
            if (lpar == NULL)
                dict_add_elem(dpar, key, val);
            else if (dpar == NULL)
                list_add_elem(lpar, val);
            else {
                error("Dawg wtf are you doing");

                json_free_token_table(tab);
                free(val);
            }

            break;
        }
        case STRING: {
            if (lpar == NULL)
                dict_add_elem(dpar, key, tok.val.strval);
            else if (dpar == NULL)
                list_add_elem(lpar, tok.val.strval);
            else {
                error("Dawg wtf are you doing");

                json_free_token_table(tab);
            }

            break;
        }
        case BOOL: {
            bool *val = (bool*)malloc(1);

            *val = tok.val.boolval;
            
            if (lpar == NULL)
                dict_add_elem(dpar, key, val);
            else if (dpar == NULL)
                list_add_elem(lpar, val);
            else {
                error("Dawg wtf are you doing");

                json_free_token_table(tab);
                free(val);
            }

            break;
        }
    }

    return i;
}

dict_t parse_json(char *pth) {
    json_token_table_t tab = json_lex(pth);

    dict_t root = dict_init();
    for (int i = 1; i < tab.tokenc - 1; i = parse_json_token(root, NULL, NULL, tab, i));

    free(tab.tokens);

    return root;
}

void free_json(dict_t json) {
    for (dict_elem_t elem = json->first; elem != NULL; elem = elem->next) {
        free(elem->key);
        free(elem->val);
    }

    free_dict(json);
}

#ifdef __cplusplus
}
#endif