#ifndef __UVRESTFUL_HTTP_FIELDS_H__
#define __UVRESTFUL_HTTP_FIELDS_H__

#include <uthash.h>

typedef struct uvr_http_fields_s uvr_http_fields;

void uvr_http_fields_set(uvr_http_fields *fields, const char *key, const char *value);
const char *uvr_http_fields_get(const uvr_http_fields *fields, const char *key);
size_t uvr_http_fields_size(const uvr_http_fields *fields);

#endif // __UVRESTFUL_HTTP_FIELDS_H__
