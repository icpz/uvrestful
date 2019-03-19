
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
    } else if (strncmp("HEAD", m, len) == 0) {
        result = UVR_METHOD_HEAD;
    }
    return result;
}

uvr_http_request_header *uvr_http_request_header_new() {
    uvr_http_request_header *p = (uvr_http_request_header *)malloc(sizeof *p);
    memset(p, 0, sizeof *p);
    utstring_new(p->uri);
    utstring_new(p->version);
    p->fields = uvr_http_fields_new();
    p->params = uvr_http_fields_new();
    return p;
}

void uvr_http_request_header_drop(uvr_http_request_header *hdr) {
    utstring_free(hdr->uri);
    utstring_free(hdr->version);
    uvr_http_fields_drop(hdr->fields);
    uvr_http_fields_drop(hdr->params);
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

static void __parse_param_string(uvr_http_fields *f, char *s) {
    const char *delim = "&";
    char *kv = NULL;
    for (kv = strtok(s, delim); kv; kv = strtok(NULL, delim)) {
        char *eq = strchr(kv, '=');
        if (!eq) {
            continue;
        }
        *eq = 0;
        uvr_http_fields_set(f, kv, eq + 1);
        *eq = '=';
    }
}

void uvr_http_request_fill_params(uvr_http_request *req) {
    char *pstart = strchr(utstring_body(req->header->uri), '?');
    if (pstart) {
        size_t ulen = pstart - utstring_body(req->header->uri);
        __parse_param_string(req->header->params, pstart + 1);
        UT_string *s, *t = NULL;
        utstring_new(s);
        utstring_bincpy(s, utstring_body(req->header->uri), ulen);
        t = req->header->uri;
        req->header->uri = s;
        s = t;
        utstring_free(s);
    }
    const char *ctype = uvr_http_fields_get(req->header->fields, "Content-Type");
    if (ctype && strcmp(ctype, "application/x-www-form-urlencoded") == 0) {
        UT_string s;
        utstring_init(&s);
        utstring_bincpy(&s, utarray_front(req->body), utarray_len(req->body));
        __parse_param_string(req->header->params, utstring_body(&s));
        utstring_done(&s);
    }
}
