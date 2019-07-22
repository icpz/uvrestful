#include <stdio.h>

#include <utstring.h>
#include <uv.h>

#include "uvrestful/restful/server.h"

void param_walk_cb(uvr_http_fields *f, const char *k, const char *v, void *arg) {
    printf("%s = %s\n", k, v);
}

void hget(const uvr_http_request *req, uvr_http_response *resp, void *arg) {
    char c = '1';
    uvr_http_fields_walk(req->header->params, param_walk_cb, NULL);
    uvr_http_fields_walk(req->header->fields, param_walk_cb, NULL);
    resp->header->status_code = 200;
    utarray_push_back(resp->body, &c);
}

void sig_cb(uv_signal_t *sig, int n) {
    uvrestful_server *s = (uvrestful_server *)sig->data;
    uvrestful_server_stop(s);
    uv_close((uv_handle_t *)sig, NULL);
}

int main(int argc, char *argv[]) {

    uv_loop_t *loop = uv_default_loop();
    uv_signal_t sig;

    uvrestful_server *s = uvrestful_server_new();
    uvrestful_server_handlers h;
    h.handle_get = hget;
    h.user_data  = NULL;

    uv_signal_init(loop, &sig);
    sig.data = s;
    uv_signal_start_oneshot(&sig, sig_cb, SIGINT);

    uvrestful_server_register(s, "/hahaha", &h);

    uvrestful_server_start(s, loop, 8998);

    uv_run(loop, UV_RUN_DEFAULT);

    uvrestful_server_drop(s, NULL);

    uv_loop_close(loop);

    return 0;
}
