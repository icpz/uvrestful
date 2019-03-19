
#include <uthash.h>

#include "uvrestful/restful/server.h"
#include "uvrestful/http/parser.h"
#include "vstr_hash_map.h"
#include "server_conn.h"

struct uvrestful_server {
    uv_tcp_t *server;
    vstr_hash_map *handlers;
    int running;
};

uvrestful_server *uvrestful_server_new(uv_loop_t *loop) {
    uvrestful_server *s = (uvrestful_server *)malloc(sizeof *s);
    s->server = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    s->handlers = vstr_hash_map_new();

    uv_tcp_init(loop, s->server);
    s->server->data = s;
    s->running = 0;

    return s;
}

static void __server_close_cb(uv_handle_t *handle) {
    uvrestful_server *s = (uvrestful_server *)handle->data;
    free(handle);
    free(s);
}

static void __server_hash_walk_cb(vstr_hash_map *h, const char *k, void *v, void *arg) {
    uvrestful_server_handlers_drop_cb cb = (uvrestful_server_handlers_drop_cb)arg;
    if (cb) {
        cb((uvrestful_server_handlers *)v);
    }
    vstr_hash_map_delete(h, k);
}

void uvrestful_server_drop(uvrestful_server *s, uvrestful_server_handlers_drop_cb cb) {
    vstr_hash_map_walk(s->handlers, __server_hash_walk_cb, cb);
    vstr_hash_map_drop(s->handlers);
    uv_close((uv_handle_t *)s->server, __server_close_cb);
}

void uvrestful_server_register(uvrestful_server *s, const char *uri, uvrestful_server_handlers *handlers) {
    vstr_hash_map_put(s->handlers, uri, handlers);
}

uvrestful_server_handlers *uvrestful_server_deregister(uvrestful_server *s, const char *uri) {
    return vstr_hash_map_release(s->handlers, uri);
}

static void __server_on_conn(uv_stream_t* server, int status);

void uvrestful_server_start(uvrestful_server *s, uint16_t port) {
    struct sockaddr_storage sock;
    s->running = 1;
    uv_ip6_addr("::", port, (struct sockaddr_in6 *)&sock);
    uv_tcp_bind(s->server, (struct sockaddr *)&sock, 0);
    uv_listen((uv_stream_t *)s->server, 2048, __server_on_conn);
}

void uvrestful_server_stop(uvrestful_server *s) {
    s->running = 0;
}

static void __server_on_conn(uv_stream_t* server, int status) {
    uvrestful_server *s = (uvrestful_server *)server->data;
    if (status < 0) {
        return;
    }
    conn_t *c = conn_new(server->loop, s->handlers);
    uv_accept(server, (uv_stream_t *)c->socket);
    conn_start(c);
}
