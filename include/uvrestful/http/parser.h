#ifndef __UVRESTFUL_HTTP_PARSER_H__
#define __UVRESTFUL_HTTP_PARSER_H__

#include "uvrestful/http/request.h"

typedef enum {
    UVR_HTTP_REQ_PARSER_OK,
    UVR_HTTP_REQ_PARSER_PARTIAL,
    UVR_HTTP_REQ_PARSER_ERROR_START, /* should never be used */
    UVR_HTTP_REQ_PARSER_EMETHOD,
    UVR_HTTP_REQ_PARSER_EVERSION,
    UVR_HTTP_REQ_PARSER_ECHARACTER
} uvr_http_request_parser_error;

typedef struct uvr_http_request_parser_s uvr_http_request_parser;

uvr_http_request_parser *uvr_http_request_parser_new();
void uvr_http_request_parser_drop(uvr_http_request_parser *parser);
uvr_http_request_parser_error \
    uvr_http_request_parser_update(uvr_http_request_parser *p, const uint8_t *data, size_t len);
uvr_http_request *uvr_http_request_parser_release(uvr_http_request_parser *parser);

#endif // __UVRESTFUL_HTTP_PARSER_H__
