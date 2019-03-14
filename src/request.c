
#include <stdlib.h>

#include "uvrestful/http/request.h"

uvr_http_method uvr_http_method_from_string(const void *m, size_t len) {
    uvr_http_method result = UVR_METHOD_UNKNOWN;
    if (strncmp("GET", m, len) == 0) {
        result = UVR_METHOD_GET;
    } else if (strncmp("POST", m, len) == 0) {
        result = UVR_METHOD_POST;
    } else if (strncmp("DELETE", m, len) == 0) {
        result = UVR_METHOD_DELETE;
    } else if (strncmp("PUT", m, len) == 0) {
        result = UVR_METHOD_PUT;
    }
    return result;
}

uvr_http_request_header *uvr_http_request_header_new() {
    uvr_http_request_header *p = (uvr_http_request_header *)malloc(sizeof *p);
    memset(p, 0, sizeof *p);
    utstring_new(p->uri);
    utstring_new(p->version);
    p->fields = uvr_http_fields_new();
    p->param = uvr_http_fields_new();
    return p;
}

void uvr_http_request_header_drop(uvr_http_request_header *hdr) {
    utstring_free(hdr->uri);
    utstring_free(hdr->version);
    uvr_http_fields_drop(hdr->fields);
    uvr_http_fields_drop(hdr->param);
    free(hdr);
}

uvr_http_request *uvr_http_request_new() {
    const UT_icd ut_uint8_icd = { sizeof(uint8_t), NULL, NULL, NULL };
    uvr_http_request *p = (uvr_http_request *)malloc(sizeof *p);
    memset(p, 0, sizeof *p);
    p->header = uvr_http_request_header_new();
    utarray_new(p->body, &ut_uint8_icd);
    return p;
}

void uvr_http_request_drop(uvr_http_request *req) {
    uvr_http_request_header_drop(req->header);
    utarray_free(req->body);
    free(req);
}

