#ifndef __UVRESTFUL_HTTP_RESPONSE_H__
#define __UVRESTFUL_HTTP_RESPONSE_H__

#include <utstring.h>
#include <utarray.h>

#include "uvrestful/http/fields.h"

typedef struct {
    int status_code;
    UT_string *version;
    uvr_http_fields *fields;
} uvr_http_response_header;

uvr_http_response_header *uvr_http_response_header_new();
void uvr_http_response_header_drop(uvr_http_response_header *h);
const char *uvr_http_response_header_message(uvr_http_response_header *h);

typedef struct {
    uvr_http_response_header *header;
    UT_array *body;
} uvr_http_response;

uvr_http_response *uvr_http_response_new();
void uvr_http_response_drop(uvr_http_response *r);
void uvr_http_response_append(uvr_http_response *r, const uint8_t *buf, size_t len);
UT_array *uvr_http_response_serialize(uvr_http_response *r);

#endif // __UVRESTFUL_HTTP_RESPONSE_H__
