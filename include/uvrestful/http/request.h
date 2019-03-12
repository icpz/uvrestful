#ifndef __UVRESTFUL_HTTP_REQUEST_H__
#define __UVRESTFUL_HTTP_REQUEST_H__

#include <utstring.h>
#include <utarray.h>

#include "uvrestful/http/fields.h"

typedef enum {
    UVR_METHOD_GET,
    UVR_METHOD_POST,
    UVR_METHOD_DELETE,
    UVR_METHOD_PUT
} uvr_http_method;

typedef struct {
    uvr_http_method method;
    UT_string *uri;
    UT_string *version;
    uvr_http_fields *fields;
} uvr_http_request_header;

uvr_http_request_header *uvr_http_request_header_new();
void uvr_http_request_header_drop(uvr_http_request_header *hdr);

typedef struct {
    uvr_http_request_header *header;
    UT_array *body;
} uvr_http_request;

uvr_http_request *uvr_http_request_new();
void uvr_http_request_drop(uvr_http_request *req);

#endif // __UVRESTFUL_HTTP_REQUEST_H__
