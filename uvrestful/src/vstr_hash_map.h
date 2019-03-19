#ifndef __UVRESTFUL_VSTR_HASH_MAP_H__
#define __UVRESTFUL_VSTR_HASH_MAP_H__

/* utstring -> void * */
typedef struct vstr_hash_map vstr_hash_map;

typedef void (*vstr_hash_map_walk_cb)(vstr_hash_map *h, const char *k, void *v, void *arg);

vstr_hash_map *vstr_hash_map_new();
void vstr_hash_map_drop(vstr_hash_map *h);

void vstr_hash_map_put(vstr_hash_map *h, const char *key, void *value);
void *vstr_hash_map_replace(vstr_hash_map *h, const char *key, void *value);
void *vstr_hash_map_get(const vstr_hash_map *h, const char *key);

void *vstr_hash_map_release(vstr_hash_map *h, const char *key);
void vstr_hash_map_delete(vstr_hash_map *h, const char *key);

void vstr_hash_map_clear(vstr_hash_map *h);
size_t vstr_hash_map_size(const vstr_hash_map *h);

void vstr_hash_map_walk(vstr_hash_map *h, vstr_hash_map_walk_cb cb, void *arg);

#endif // __UVRESTFUL_VSTR_HASH_MAP_H__
