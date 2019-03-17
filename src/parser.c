
#include <stdlib.h>

#include <utarray.h>

#include "uvrestful/http/request.h"
#include "uvrestful/http/parser.h"

typedef enum {
    PARSING_METHOD,
    PARSING_URI,
    PARSING_VERSION,
    PARSING_FIELDS,
    PARSING_DONE
} parser_state;

struct uvr_http_request_parser_s {
    parser_state state;
    UT_array *buffer;
    uvr_http_request *request;
};

uvr_http_request_parser *uvr_http_request_parser_new() {
    const UT_icd ut_uint8_icd = { sizeof(uint8_t), NULL, NULL, NULL };
    uvr_http_request_parser *p = (uvr_http_request_parser *)malloc(sizeof *p);
    p->state = PARSING_METHOD;
    utarray_new(p->buffer, &ut_uint8_icd);
    p->request = uvr_http_request_new();
    return p;
}

void uvr_http_request_parser_drop(uvr_http_request_parser *p) {
    utarray_free(p->buffer);
    if (p->request) {
        uvr_http_request_drop(p->request);
    }
    free(p);
}

static const uint8_t *__find_first(UT_array *a, uint8_t u) {
    int i;
    for (i = 0; i < utarray_len(a); ++i) {
        uint8_t *c = (uint8_t *)utarray_eltptr(a, i);
        if (*c == u) {
            return c;
        }
    }
    return NULL;
}

static const uint8_t *__find_line_end(UT_array *a, int *invalid) {
    uint8_t *head = (uint8_t *)utarray_front(a);
    size_t len = utarray_len(a);
    size_t i = 0;

    *invalid = 0;
    while (i < len - 1 && head[i] != '\r' && head[i + 1] != '\n') {
        if (head[i] == '\r' || head[i] == '\n') {
            *invalid = 1;
            return NULL;
        }
        ++i;
    }
    if (i >= len - 1) {
        return NULL;
    }
    return head + i;
}

static uvr_http_request_parser_error __try_next_stage(uvr_http_request_parser *p, int *done) {
    uvr_http_request_parser_error ret = UVR_HTTP_REQ_PARSER_PARTIAL;
    const uint8_t *pos = NULL;
    int err = 0;
    *done = 0;
    switch (p->state) {
    case PARSING_METHOD:
        if (!p->request) {
            p->request = uvr_http_request_new();
        }
        pos = __find_first(p->buffer, (uint8_t)' ');
        if (pos) {
            uint8_t *head = (uint8_t *)utarray_front(p->buffer);
            size_t method_len = pos - head;
            *done = 1;
            p->state = PARSING_URI;

            p->request->header->method = uvr_http_method_from_string(head, method_len);
            if (p->request->header->method == UVR_METHOD_UNKNOWN) {
                ret = UVR_HTTP_REQ_PARSER_EMETHOD;
                goto __next_stage_end;
            }
            utarray_erase(p->buffer, 0, method_len + 1);
        }
        break;
    case PARSING_URI:
        pos = __find_first(p->buffer, (uint8_t)' ');
        if (pos) {
            uint8_t *head = (uint8_t *)utarray_front(p->buffer);
            size_t uri_len = pos - head;
            *done = 1;
            p->state = PARSING_VERSION;

            utstring_bincpy(p->request->header->uri, head, uri_len);
            utarray_erase(p->buffer, 0, uri_len + 1);
        }
        break;
    case PARSING_VERSION:
        pos = __find_line_end(p->buffer, &err);
        if (err) {
            ret = UVR_HTTP_REQ_PARSER_ECHARACTER;
            goto __next_stage_end;
        }
        if (pos) {
            uint8_t *head = (uint8_t *)utarray_front(p->buffer);
            size_t ver_len = pos - head;
            *done = 1;
            p->state = PARSING_FIELDS;

            utstring_bincpy(p->request->header->version, head, ver_len);
            utarray_erase(p->buffer, 0, ver_len + 2);
        }
        break;
    case PARSING_FIELDS:
        pos = __find_line_end(p->buffer, &err);
        if (err) {
            ret = UVR_HTTP_REQ_PARSER_ECHARACTER;
            goto __next_stage_end;
        }
        if (pos) {
            const uint8_t *head = (uint8_t *)utarray_front(p->buffer);
            size_t field_len = pos - head;
            int err = 0;
            *done = 1;

            if (field_len == 0) {
                p->state = PARSING_DONE;
            } else {
                p->state = PARSING_FIELDS;
                uvr_http_fields_parse_header(p->request->header->fields, (const char *)head, field_len, &err);
                if (err) {
                    ret = UVR_HTTP_REQ_PARSER_ECHARACTER;
                    goto __next_stage_end;
                }
            }
            utarray_erase(p->buffer, 0, field_len + 2);
        }
        break;
    case PARSING_DONE:
        *done = 1;
        ret = UVR_HTTP_REQ_PARSER_OK;
        utarray_concat(p->request->body, p->buffer);
        utarray_clear(p->buffer);
        break;
    default:
        break;
    }
__next_stage_end:
    return ret;
}

uvr_http_request_parser_error \
    uvr_http_request_parser_update(uvr_http_request_parser *p, const uint8_t *data, size_t len) {
        uvr_http_request_parser_error ret = UVR_HTTP_REQ_PARSER_PARTIAL;
        int i, done;
        UT_array *dest = p->buffer;
        if (p->state == PARSING_DONE) {
            ret = UVR_HTTP_REQ_PARSER_OK;
            dest = p->request->body;
        }
        for (i = 0; i < len; ++i) {
            utarray_push_back(dest, data + i);
        }
        if (p->state != PARSING_DONE) {
            while ((ret = __try_next_stage(p, &done)) == UVR_HTTP_REQ_PARSER_PARTIAL) {
                if (!done) {
                    break;
                }
            }
        }
        return ret;
    }

uvr_http_request *uvr_http_request_parser_release(uvr_http_request_parser *parser) {
    uvr_http_request *r = parser->request;
    parser->request = NULL;
    return r;
}

uvr_http_request *uvr_http_request_parser_peek(uvr_http_request_parser *parser) {
    return parser->request;
}

