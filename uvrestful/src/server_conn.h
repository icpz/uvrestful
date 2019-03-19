#ifndef __UVRESTFUL_SERVER_CONN_H__
#define __UVRESTFUL_SERVER_CONN_H__

#include <uv.h>

#include "uvrestful/http/request.h"
#include "uvrestful/http/response.h"
#include "uvrestful/http/parser.h"
#include "uvrestful/restful/server.h"
#include "vstr_hash_map.h"

typedef struct {
    uv_tcp_t *socket;
    uv_buf_t uvbuf;
    uv_write_t write_req;
    uvr_http_request_parser *parser;
    uvr_http_request *req;
    uvr_http_response *resp;
    UT_string *resp_buf;
    vstr_hash_map *handlers;
    int closing;
    char buf[2048];
} conn_t;

conn_t *conn_new(uv_loop_t *loop, vstr_hash_map *handlers);
void conn_drop(conn_t *c);
int conn_start(conn_t *c);

#endif // __UVRESTFUL_SERVER_CONN_H__
