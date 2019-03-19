
#define HASH_FUNCTION(keyptr, keylen, hashv) \
    HASH_JEN(utstring_body((UT_string *)keyptr), keylen, hashv)
#define HASH_KEYCMP(a, b, n) \
    memcmp(utstring_body((UT_string *)a), utstring_body((UT_string *)b), n)
#include <uthash.h>
#include <utstring.h>

#include "vstr_hash_map.h"

typedef struct {
    UT_hash_handle hh;
    UT_string key;
    void *value;
} __hash_node;

struct vstr_hash_map {
    __hash_node *head;
};

static __hash_node *__hash_node_new(const char *k, void *v) {
    __hash_node *n = (__hash_node *)malloc(sizeof *n);
    memset(n, 0, sizeof *n);
    utstring_init(&n->key);
    utstring_bincpy(&n->key, k, strlen(k));
    n->value = v;
    return n;
}

static void __hash_node_drop(__hash_node *n) {
    utstring_done(&n->key);
    free(n);
}

vstr_hash_map *vstr_hash_map_new() {
    vstr_hash_map *h = (vstr_hash_map *)malloc(sizeof *h);
    memset(h, 0, sizeof *h);
    return h;
}

void vstr_hash_map_drop(vstr_hash_map *h) {
    vstr_hash_map_clear(h);
    free(h);
}

void vstr_hash_map_put(vstr_hash_map *h, const char *key, void *value) {
    __hash_node *n = __hash_node_new(key, value);
    n->value = value;
    HASH_ADD(hh, h->head, key, utstring_len(&n->key), n);
}

void *vstr_hash_map_replace(vstr_hash_map *h, const char *key, void *value) {
    __hash_node *n = __hash_node_new(key, value), *exists = NULL;
    void *result = NULL;
    HASH_REPLACE(hh, h->head, key, utstring_len(&n->key), n, exists);
    if (exists) {
        result = exists->value;
        __hash_node_drop(exists);
    }
    return result;
}

static __hash_node *__hash_find_node(const vstr_hash_map *h, const char *key) {
    __hash_node *n = NULL;
    UT_string tkey;
    utstring_init(&tkey);
    utstring_bincpy(&tkey, key, strlen(key));

    HASH_FIND(hh, h->head, &tkey, utstring_len(&tkey), n);
    utstring_done(&tkey);
    return n;
}

void *vstr_hash_map_get(const vstr_hash_map *h, const char *key) {
    __hash_node *n = __hash_find_node(h, key);
    return (n ? n->value : n);
}

void *vstr_hash_map_release(vstr_hash_map *h, const char *key) {
    __hash_node *n = __hash_find_node(h, key);
    void *result = NULL;
    if (n) {
        result = n->value;
        HASH_DELETE(hh, h->head, n);
        __hash_node_drop(n);
    }
    return result;
}

void vstr_hash_map_delete(vstr_hash_map *h, const char *key) {
    vstr_hash_map_release(h, key);
}

void vstr_hash_map_clear(vstr_hash_map *h) {
    __hash_node *n, *tmp = NULL;
    HASH_ITER(hh, h->head, n, tmp) {
        HASH_DELETE(hh, h->head, n);
        __hash_node_drop(n);
    }
}

size_t vstr_hash_map_size(const vstr_hash_map *h) {
    return HASH_COUNT(h->head);
}

void vstr_hash_map_walk(vstr_hash_map *h, vstr_hash_map_walk_cb cb, void *arg) {
    __hash_node *n, *tmp = NULL;
    HASH_ITER(hh, h->head, n, tmp) {
        cb(h, utstring_body(&n->key), n->value, arg);
    }
}

