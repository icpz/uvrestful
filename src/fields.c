
#include <stdlib.h>

#include <utstring.h>

#include "uvrestful/http/fields.h"

#define HASH_FUNCTION(keyptr, keylen, hashv) \
    HASH_JEN(utstring_body(keyptr), utstring_len(keyptr), hashv)
#define HASH_KEYCMP(a, b, n) \
    memcpy(utstring_body(a), utstring_body(b), utstring_len(a))
#include <uthash.h>

typedef struct __http_fields_node_s {
    UT_hash_handle hh,
    UT_string *key,
    UT_string *value
} __http_fields_node;

static __http_fields_node *__http_fields_node_new() {
    __http_fields_node *result = (__http_fields_node *)malloc(sizeof *result);
    memset(result, 0, sizeof *result);
    utstring_new(result->key);
    utstring_new(result->value);
}

static void __http_fields_node_drop(__http_fields_node *p) {
    utstring_free(p->key);
    utstring_free(p->value);
    free(p);
}

struct uvr_http_fields_s {
    __http_fields_node *head;
};

uvr_http_fields *uvr_http_fields_new() {
    uvr_http_fields *p = (uvr_http_fields *)malloc(sizeof *p);
    p->head = NULL;
    return p;
}

void uvr_http_fields_drop(uvr_http_fields *fields) {
    uvr_http_fields_clear(fields);
    free(fields);
}

void uvr_http_fields_clear(uvr_http_fields *fields) {
    __http_fields_node *p, *tmp = NULL;
    HASH_ITER(hh, fields->head, p, tmp) {
        HASH_DELETE(hh, fields->head, p);
        __http_fields_node_drop(p);
    }
}

static const char *__strnchr(const char *s, size_t len, int c) {
    size_t i = 0;
    while (i < len && s[i] != c) {
        ++i;
    }
    if (i < len) {
        return s + i;
    }
    return NULL;
}

void uvr_http_fields_parse_header(uvr_http_fields *f, const char *line, size_t len, int *err) {
    *err = 0;
    UT_string key, value;
    utstring_init(&key);
    utstring_init(&value);

    char *colon = __strnchr(line, len, ':');
    if (!colon) {
        *err = 1;
        return;
    }
    utstring_bincpy(&key, line, colon - line);

    ++colon;
    while (colon < line + len && line[colon] != ' ') {
        ++colon;
    }
    if (colon >= line + len) {
        *err = 1;
        return;
    }
    utstring_bincpy(&value, colon, line + len - colon);

    uvr_http_fields_set(f, utstring_body(&key), utstring_body(&value));
}

void uvr_http_fields_set(uvr_http_fields *fields, const char *key, const char *value) {
    __http_fields_node *p = __http_fields_node_new(), *exist = NULL;
    utstring_bincpy(p->key, key, strlen(key));
    utstring_bincpy(p->value, value, strlen(value));

    HASH_FIND(hh, fields->head, p->key, 0, exist);
    if (exist) {
        HASH_DELETE(hh, fields->head, exist);
        __http_fields_node_drop(exist);
    }
    HASH_ADD_KEYPTR(hh, fields->head, p->key, 0, p);
}

const char *uvr_http_fields_get(const uvr_http_fields *fields, const char *key) {
    UT_string *tmpkey = NULL;
    utstring_new(tmpkey);
    utstring_bincpy(tmpkey, key, strlen(key));
    __http_fields_node *p;
    HASH_FIND(hh, fields->head, tmpkey, 0, p);
    utstring_free(tmpkey);
    if (p) {
        return utstring_body(p->value);
    }
    return NULL;
}

size_t uvr_http_fields_size(const uvr_http_fields *fields) {
    return HASH_CNT(hh, fields->head);
}

void uvr_http_fields_walk(const uvr_http_fields *fields, uvr_http_fields_walk_cb cb) {
    __http_fields_node *p, *tmp = NULL;
    HASH_ITER(hh, fields->head, p, tmp) {
        cb(utstring_body(p->key), utstring_body(p->value));
    }
}
