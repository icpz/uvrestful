#ifndef __UVRESTFUL_HTTP_REQUEST_H__
#define __UVRESTFUL_HTTP_REQUEST_H__

#include <uthash.h>
#include <utstring.h>
#include <utarray.h>

#include "fields.h"

typedef enum {
    UVR_GET,
    UVR_POST,
    UVR_DELETE,
    UVR_PUT
} uvr_http_method;

typedef struct {
    uvr_http_method method;
    UT_string *uri;
    UT_string *version;
    uvr_http_fields *fields;
} uvr_http_request_header;

typedef struct {
    uvr_http_request_header header;
    UT_array *body;
} uvr_http_request;

uvr_http_method uvr_http_request_method(const uvr_http_request *req);
const char *uvr_http_request_uri(const uvr_http_request *req);
const char *uvr_http_request_field(const uvr_http_request *req);
const uint8_t *uvr_http_request_body(const uvr_http_request *req);
size_t *uvr_http_request_body_size(const uvr_http_request *req);

#endif // __UVRESTFUL_HTTP_REQUEST_H__
