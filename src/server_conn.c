
#include "server_conn.h"

conn_t *conn_new(uv_loop_t *loop, vstr_hash_map *h) {
    conn_t *c = (conn_t *)malloc(sizeof *c);
    memset(c, 0, sizeof *c);

    c->socket   = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    c->parser   = uvr_http_request_parser_new();
    c->resp     = uvr_http_response_new();
    c->handlers = h;

    uv_tcp_init(loop, c->socket);
    c->socket->data = c;
    c->write_req.data = c;

    return c;
}

static void __socket_close_cb(uv_handle_t *handle) {
    conn_t *conn = (conn_t *)handle->data;
    free(handle);
    conn->closing++;
    if (conn->closing == 2) {
        free(conn);
    }
}

void conn_drop(conn_t *c) {
    if (c->closing) { return; }
    c->closing = 1;
    uvr_http_request_parser_drop(c->parser);
    uvr_http_response_drop(c->resp);
    if (c->req) {
        uvr_http_request_drop(c->req);
    }
    if (c->resp_buf) {
        utstring_free(c->resp_buf);
    }
    uv_close((uv_handle_t *)c->socket, __socket_close_cb);
}

static void __conn_read_done(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
static void __default_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

int conn_start(conn_t *c) {
    return uv_read_start((uv_stream_t *)c->socket, __default_alloc_cb, __conn_read_done);
}

static void __default_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    conn_t *c = (conn_t *)handle->data;
    buf->base = c->buf;
    buf->len  = sizeof c->buf;
}

static void __conn_call_handler(conn_t *c);
static void __conn_write_resp(conn_t *c);

static void __conn_read_done(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    conn_t *c = (conn_t *)stream->data;
    uvr_http_request_parser_error pe = 0;
    if (nread == 0) {
        return;
    }
    if (nread < 0) {
        uv_read_stop(stream);
        if (nread != UV_EOF) {
            conn_drop(c);
            return;
        }
    }
    pe = uvr_http_request_parser_update(c->parser, (uint8_t *)buf->base, nread);
    if (pe == UVR_HTTP_REQ_PARSER_PARTIAL) {
        return;
    } else if (pe == UVR_HTTP_REQ_PARSER_OK) {
        uvr_http_request *r = uvr_http_request_parser_peek(c->parser);
        const char *csize_buf = uvr_http_fields_get(r->header->fields, "Content-Length");
        size_t expected_csize = 0;
        if (csize_buf) {
            expected_csize = (size_t)atol(csize_buf);
        }
        if (expected_csize > utarray_len(r->body)) {
            return;
        }
        c->req = uvr_http_request_parser_release(c->parser);
        __conn_call_handler(c);
    } else {
        utstring_clear(c->resp->header->version);
        utstring_concat(c->resp->header->version, c->req->header->version);
        c->resp->header->status_code = 400;
    }
    uv_read_stop(stream);
    __conn_write_resp(c);
}

static void __conn_call_handler(conn_t *c) {
    uvr_http_method method = c->req->header->method;
    uvrestful_server_handlers *h = \
        vstr_hash_map_get(c->handlers, utstring_body(c->req->header->uri));
    utstring_clear(c->resp->header->version);
    utstring_concat(c->resp->header->version, c->req->header->version);
    if (!h) {
        c->resp->header->status_code = 404;
    } else {
        uvrestful_server_method_handler handle = NULL;
        int invalid_method = 0;
        c->resp->header->status_code = 200;
        switch(method) {
        case UVR_METHOD_GET:
            handle = h->handle_get;
            break;
        case UVR_METHOD_POST:
            handle = h->handle_post;
            break;
        case UVR_METHOD_PUT:
            handle = h->handle_put;
            break;
        case UVR_METHOD_DELETE:
            handle = h->handle_delete;
            break;
        default:
            invalid_method = 1;
            break;
        }
        if (invalid_method) {
            c->resp->header->status_code = 405;
        } else if (!handle) {
            c->resp->header->status_code = 501;
        } else {
            handle(c->req, c->resp, h->user_data);
        }
    }
}

static void __conn_write_done(uv_write_t* req, int status) {
    conn_t *c = (conn_t *)req->data;
    if (status == UV_ECANCELED) {
        return;
    }
    conn_drop(c);
}

static void __conn_write_resp(conn_t *c) {
    c->resp_buf   = uvr_http_response_serialize(c->resp);
    c->uvbuf.base = utstring_body(c->resp_buf);
    c->uvbuf.len  = utstring_len(c->resp_buf);
    int ret = 0;
    ret = uv_write(&c->write_req, (uv_stream_t *)c->socket, &c->uvbuf, 1, __conn_write_done);
    if (ret) {
        conn_drop(c);
    }
}
