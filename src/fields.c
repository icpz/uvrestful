
#include <stdlib.h>

#include <utstring.h>

#include "uvrestful/http/fields.h"
#include "vstr_hash_map.h"

struct uvr_http_fields_s {
    vstr_hash_map *head;
};

uvr_http_fields *uvr_http_fields_new() {
    uvr_http_fields *p = (uvr_http_fields *)malloc(sizeof *p);
    p->head = vstr_hash_map_new();
    return p;
}

void uvr_http_fields_drop(uvr_http_fields *fields) {
    uvr_http_fields_clear(fields);
    vstr_hash_map_drop(fields->head);
    free(fields);
}

static void __fields_clear_cb(vstr_hash_map *h, const char *k, void *v, void *arg) {
    UT_string *s = (UT_string *)vstr_hash_map_release(h, k);
    utstring_free(s);
}

void uvr_http_fields_clear(uvr_http_fields *fields) {
    vstr_hash_map_walk(fields->head, __fields_clear_cb, NULL);
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

    const char *colon = __strnchr(line, len, ':');
    if (!colon) {
        *err = 1;
        return;
    }
    utstring_bincpy(&key, line, colon - line);

    ++colon;
    while (colon < line + len && *colon == ' ') {
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
    UT_string *v = NULL;
    utstring_new(v);
    utstring_bincpy(v, value, strlen(value));

    vstr_hash_map_replace(fields->head, key, v);
}

const char *uvr_http_fields_get(const uvr_http_fields *fields, const char *key) {
    void *v = vstr_hash_map_get(fields->head, key);
    return (v ? utstring_body((UT_string *)v) : v);
}

size_t uvr_http_fields_size(const uvr_http_fields *fields) {
    return vstr_hash_map_size(fields->head);
}

struct walk_ctx {
    uvr_http_fields_walk_cb cb;
    void *arg;
};

static void __fields_walk_cb(vstr_hash_map *h, const char *k, void *v, void *arg) {
    struct walk_ctx *ctx = (struct walk_ctx *)arg;
    ctx->cb(k, utstring_body((UT_string *)v), ctx->arg);
}

void uvr_http_fields_walk(const uvr_http_fields *fields, uvr_http_fields_walk_cb cb, void *arg) {
    struct walk_ctx ctx;
    ctx.cb = cb;
    ctx.arg = arg;
    vstr_hash_map_walk(fields->head, __fields_walk_cb, &ctx);
}
