#ifndef __UVRESTFUL_RESTFUL_SERVER_H__
#define __UVRESTFUL_RESTFUL_SERVER_H__

#include <uv.h>

#include "uvrestful/http/request.h"
#include "uvrestful/http/response.h"

typedef struct uvrestful_server uvrestful_server;

typedef void (*uvrestful_server_method_handler)(const uvr_http_request *req, uvr_http_response *resp, void *arg);

typedef struct {
    uvrestful_server_method_handler handle_get;
    uvrestful_server_method_handler handle_post;
    uvrestful_server_method_handler handle_delete;
    uvrestful_server_method_handler handle_put;
    void *user_data;
} uvrestful_server_handlers;

typedef void (*uvrestful_server_handlers_drop_cb)(uvrestful_server_handlers *h);

uvrestful_server *uvrestful_server_new();
void uvrestful_server_drop(uvrestful_server *s, uvrestful_server_handlers_drop_cb cb);
void uvrestful_server_register(uvrestful_server *s, const char *uri, uvrestful_server_handlers *handlers);
uvrestful_server_handlers *uvrestful_server_deregister(uvrestful_server *s, const char *uri);
void uvrestful_server_start(uvrestful_server *s, uv_loop_t *loop, uint16_t port);
void uvrestful_server_stop(uvrestful_server *s);

#endif // __UVRESTFUL_RESTFUL_SERVER_H__
