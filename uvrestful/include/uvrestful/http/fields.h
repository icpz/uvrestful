#ifndef __UVRESTFUL_HTTP_FIELDS_H__
#define __UVRESTFUL_HTTP_FIELDS_H__

typedef struct uvr_http_fields_s uvr_http_fields;
typedef void (*uvr_http_fields_walk_cb)(uvr_http_fields *f, const char *k, const char *v, void *arg);

uvr_http_fields *uvr_http_fields_new();
void uvr_http_fields_drop(uvr_http_fields *fields);
void uvr_http_fields_clear(uvr_http_fields *fields);
void uvr_http_fields_parse_header(uvr_http_fields *fields, const char *line, size_t len, int *err);
void uvr_http_fields_set(uvr_http_fields *fields, const char *key, const char *value);
const char *uvr_http_fields_get(const uvr_http_fields *fields, const char *key);
size_t uvr_http_fields_size(const uvr_http_fields *fields);
void uvr_http_fields_walk(uvr_http_fields *fields, uvr_http_fields_walk_cb cb, void *arg);

#endif // __UVRESTFUL_HTTP_FIELDS_H__
