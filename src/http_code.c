#include "http_code.h"

static const char *dicts[600];
static int dicts_initialized = 0;

#define DEF_CODE_MSG(code, msg) dicts[code]=#msg

static void init_dicts() {
    DEF_CODE_MSG(100, Continue);
    DEF_CODE_MSG(101, Switching Protocols);

    DEF_CODE_MSG(200, OK);
    DEF_CODE_MSG(201, Created);
    DEF_CODE_MSG(202, Accepted);
    DEF_CODE_MSG(203, Non-Authoritative Information);
    DEF_CODE_MSG(204, No Content);
    DEF_CODE_MSG(205, Reset Content);
    DEF_CODE_MSG(206, Partial Content);

    DEF_CODE_MSG(300, Multiple Choices);
    DEF_CODE_MSG(301, Moved Permanently);
    DEF_CODE_MSG(302, Found);
    DEF_CODE_MSG(303, See Other);
    DEF_CODE_MSG(304, Not Modified);
    DEF_CODE_MSG(305, Use Proxy);
    DEF_CODE_MSG(307, Temporary Redirect);

    DEF_CODE_MSG(400, Bad Request);
    DEF_CODE_MSG(401, Unauthorized);
    DEF_CODE_MSG(402, Payment Required);
    DEF_CODE_MSG(403, Forbidden);
    DEF_CODE_MSG(404, Not Found);
    DEF_CODE_MSG(405, Method Not Allowed);
    DEF_CODE_MSG(406, Not Acceptable);
    DEF_CODE_MSG(407, Proxy Authentication Required);
    DEF_CODE_MSG(408, Request Timeout);
    DEF_CODE_MSG(409, Conflict);
    DEF_CODE_MSG(410, Gone);
    DEF_CODE_MSG(411, Length Required);
    DEF_CODE_MSG(412, Precondition Failed);
    DEF_CODE_MSG(413, Request Entity Too Large);
    DEF_CODE_MSG(414, Request-URI Too Long);
    DEF_CODE_MSG(415, Unsupported Media Type);
    DEF_CODE_MSG(416, Requested Range Not Satisfiable);
    DEF_CODE_MSG(417, Expectation Failed);

    DEF_CODE_MSG(500, Internal Server Error);
    DEF_CODE_MSG(501, Not Implemented);
    DEF_CODE_MSG(502, Bad Gateway);
    DEF_CODE_MSG(503, Service Unavailable);
    DEF_CODE_MSG(504, Gateway Timeout);
    DEF_CODE_MSG(505, HTTP Version Not Supported);
}

const char *get_message_from_code(int code, const char *default_message) {
    if (!dicts_initialized) {
        init_dicts();
        dicts_initialized = 1;
    }
    return dicts[code] ? dicts[code] : default_message;
}

