
#include "uvrestful/http/response.h"

#include "http_code.h"

uvr_http_response_header *uvr_http_response_header_new() {
    uvr_http_response_header *h = NULL;
    h = (uvr_http_response_header *)malloc(sizeof *h);
    h->status_code = 0;
    utstring_new(h->version);
    h->fields = uvr_http_fields_new();
    return h;
}

void uvr_http_response_header_drop(uvr_http_response_header *h) {
    utstring_free(h->version);
    uvr_http_fields_drop(h->fields);
    free(h);
}

const char *uvr_http_response_header_message(uvr_http_response_header *h) {
    return get_message_from_code(h->status_code, "");
}

uvr_http_response *uvr_http_response_new() {
    UT_icd ut_uint8_icd = { sizeof(uint8_t), NULL, NULL, NULL };
    uvr_http_response *r = NULL;
    r = (uvr_http_response *)malloc(sizeof *r);
    r->header = uvr_http_response_header_new();
    utarray_new(r->body, &ut_uint8_icd);
    return r;
}

void uvr_http_response_drop(uvr_http_response *r) {
    uvr_http_response_header_drop(r->header);
    utarray_free(r->body);
    free(r);
}

void uvr_http_response_append(uvr_http_response *r, const uint8_t *buf, size_t len) {
    size_t i = 0;
    for (i = 0; i < len; ++i) {
        utarray_push_back(r->body, buf + i);
    }
}

static void __kv_walk_cb(uvr_http_fields *f, const char *key, const char *value, void *arg) {
    UT_string *s = (UT_string *)arg;
    utstring_printf(s, "%s: %s\r\n", key, value);
}

UT_array *uvr_http_response_serialize(uvr_http_response *r) {
    UT_icd ut_uint8_icd = { sizeof(uint8_t), NULL, NULL, NULL };
    UT_array *res = NULL;
    UT_string hbuf;
    int i = 0;

    utarray_new(res, &ut_uint8_icd);
    utstring_init(&hbuf);

    utstring_printf(
        &hbuf, "%s %d %s\r\n",
        utstring_body(r->header->version),
        r->header->status_code,
        uvr_http_response_header_message(r->header)
    );
    if (utarray_len(r->body)) {
        char content_len_buf[64] = "";
        snprintf(content_len_buf, sizeof content_len_buf, "%zu", (size_t)utarray_len(r->body));
        uvr_http_fields_set(r->header->fields, "Content-Length", content_len_buf);
    }
    uvr_http_fields_walk(r->header->fields, __kv_walk_cb, &hbuf);
    utstring_printf(&hbuf, "\r\n");

    for (i = 0; i < utstring_len(&hbuf); ++i) {
        utarray_push_back(res, utstring_body(&hbuf) + i);
    }
    utstring_done(&hbuf);
    if (utarray_len(r->body)) {
        utarray_concat(res, r->body);
    }
    return res;
}
