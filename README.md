# Ys

Ys is a modern, minimal web application framework for the C programming language. With Ys, you can bootstrap functional APIs with minimal dev effort using a rich feature-set of high-level utilities.

[Read the docs](https://exbotanical.github.io/ys)

## Code Examples
With Ys you can do something like this

```c
#include "libys.h"
// ...

ys_response *handler(ys_request *req, ys_response *res) {
  ys_set_header(res, "Content-Type", "text/plain");

  ys_cookie *c = ys_cookie_init(COOKIE_ID, sid);
  ys_cookie_set_expires(c, ys_n_minutes_from_now(10));
  ys_set_cookie(res, c);

  ys_set_body(res, "Hello World!");
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

int main() {
  ys_router_attr *attr = ys_router_attr_init();
  ys_use_cors(attr, ys_cors_allow_all());
  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/", handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(router, PORT);
  ys_server_start(server);

  return EXIT_SUCCESS;
}
```

Auth with Cookies:

```c
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uuid/uuid.h>

#include "deps/jsob/jsob.h"
#include "libhash/libhash.h"
#include "libys.h"

#define SESSION_TIMEOUT_MINUTES 10

static pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t user_mutex = PTHREAD_MUTEX_INITIALIZER;

hash_table *session_store;
hash_table *user_store;

char *COOKIE_ID = "DemoCookie";

const int PASSWORD_HASH_KEY = 7;

typedef struct {
  char *username;
  char *password;
} user;

bool eq(const char *s1, const char *s2) { return strcmp(s1, s2) == 0; }

char *create_sid(void) {
  char uuid[UUID_STR_LEN];

  uuid_t bin_uuid;
  uuid_generate_random(bin_uuid);
  uuid_unparse(bin_uuid, uuid);

  return strdup(uuid);
}

user *create_user(char *username, char *password) {
  user *u = malloc(sizeof(user));
  u->username = username;
  u->password = password;

  return u;
}

bool user_exists(char *username) {
  pthread_mutex_lock(&user_mutex);
  bool ret = ht_search(user_store, username) != NULL;
  pthread_mutex_unlock(&user_mutex);

  return ret;
}

// This is for the demo ONLY! Do NOT use this for actual auth - it is NOT
// secure!!!
char *hash_password(char *pw) {
  char ret[sizeof(pw) + 1];

  int i = 0;
  for (; i < strlen(pw); i++) {
    ret[i] = (pw[i] ^ PASSWORD_HASH_KEY);
  }

  ret[i] = '\0';

  return strdup(ret);
}

ys_response *index_handler(ys_request *req, ys_response *res) {
  ys_set_status(res, YS_STATUS_OK);
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_HTML);
  ys_set_body(res, ys_from_file("./index.html"));

  return res;
}

ys_response *css_handler(ys_request *req, ys_response *res) {
  ys_set_status(res, YS_STATUS_OK);
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_CSS);
  ys_set_body(res, ys_from_file("./style.css"));

  return res;
}

ys_response *data_handler(ys_request *req, ys_response *res) {
  ys_cookie *c = ys_get_cookie(req, COOKIE_ID);
  char *sid = ys_cookie_get_value(c);

  pthread_mutex_lock(&session_mutex);
  char *username = ht_get(session_store, sid);
  pthread_mutex_unlock(&session_mutex);

  ys_set_body(res, "{ \"data\": \"Hello, %s!\" }", username);
  ys_set_header(res, "Content-Type", YS_MIME_TYPE_JSON);
  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *register_handler(ys_request *req, ys_response *res) {
  char *username = jsob_getstr(ys_req_get_body(req), "username");
  if (!username) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "Must provide a username");
    return res;
  }

  char *password = jsob_getstr(ys_req_get_body(req), "password");
  if (!password) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "Must provide a password");
    return res;
  }

  if (user_exists(username)) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "Username %s exists", username);
    return res;
  }

  user *u = create_user(username, hash_password(password));

  pthread_mutex_lock(&user_mutex);
  ht_insert(user_store, username, u);
  pthread_mutex_unlock(&user_mutex);

  char *sid = create_sid();

  pthread_mutex_lock(&session_mutex);
  ht_insert(session_store, sid, username);
  pthread_mutex_unlock(&session_mutex);

  ys_cookie *c = ys_cookie_init(COOKIE_ID, sid);
  ys_cookie_set_expires(c, ys_n_minutes_from_now(SESSION_TIMEOUT_MINUTES));

  ys_set_cookie(res, c);

  ys_set_status(res, YS_STATUS_CREATED);
  return res;
}

ys_response *login_handler(ys_request *req, ys_response *res) {
  char *username = jsob_getstr(ys_req_get_body(req), "username");
  if (!username) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "Must provide a username");
    return res;
  }

  char *password = jsob_getstr(ys_req_get_body(req), "password");
  if (!password) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "Must provide a password");
    return res;
  }

  if (!user_exists(username)) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "invalid credentials");

    return res;
  }

  pthread_mutex_lock(&user_mutex);
  user *u = (char *)ht_get(user_store, username);
  pthread_mutex_unlock(&user_mutex);

  if (!u || !s_equals(hash_password(u->password), password)) {
    ys_set_status(res, YS_STATUS_BAD_REQUEST);
    ys_set_body(res, "invalid credentials");

    return res;
  }

  char *session_id = create_sid();

  pthread_mutex_lock(&session_mutex);
  ht_insert(session_store, session_id, username);
  pthread_mutex_unlock(&session_mutex);

  ys_cookie *c = ys_cookie_init(COOKIE_ID, session_id);
  ys_cookie_set_expires(c, ys_n_minutes_from_now(SESSION_TIMEOUT_MINUTES));
  ys_cookie_set_path(c, "/");
  ys_set_cookie(res, c);

  ys_set_status(res, YS_STATUS_OK);

  return res;
}

ys_response *logout_handler(ys_request *req, ys_response *res) {
  ys_cookie *c = ys_get_cookie(req, COOKIE_ID);
  if (!c) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  char *sid = ys_cookie_get_value(c);
  if (!sid) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  pthread_mutex_lock(&session_mutex);
  ht_delete(session_store, sid);
  pthread_mutex_unlock(&session_mutex);

  ys_cookie_set_max_age(c, -1);
  ys_set_cookie(res, c);

  return res;
}

ys_response *auth_middleware(ys_request *req, ys_response *res) {
  ys_set_header(res, "X-Authorized-By", "TheDemoApp");

  ys_cookie *c = ys_get_cookie(req, COOKIE_ID);
  if (!c) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  char *sid = ys_cookie_get_value(c);

  pthread_mutex_lock(&session_mutex);
  char *username = ht_get(session_store, sid);
  pthread_mutex_unlock(&session_mutex);

  if (!username || !user_exists(username)) {
    ys_set_status(res, YS_STATUS_UNAUTHORIZED);
    ys_set_done(res);

    return res;
  }

  ys_cookie_set_expires(c, ys_n_minutes_from_now(SESSION_TIMEOUT_MINUTES));
  ys_set_cookie(res, c);

  return res;
}

int main() {
  session_store = ht_init(0);
  user_store = ht_init(0);

  ys_router_attr *attr = ys_router_attr_init();
  ys_add_middleware_with_opts(attr, auth_middleware, "^/$", "^/style.css$",
                              "^/login$", "^/register$");

  ys_router *router = ys_router_init(attr);

  ys_router_register(router, "/register", register_handler, YS_METHOD_POST);
  ys_router_register(router, "/login", login_handler, YS_METHOD_POST);
  ys_router_register(router, "/logout", logout_handler, YS_METHOD_POST);
  ys_router_register(router, "/data", data_handler, YS_METHOD_GET);

  ys_router_register(router, "/", index_handler, YS_METHOD_GET);
  ys_router_register(router, "/style.css", css_handler, YS_METHOD_GET);

  ys_server *server = ys_server_init(ys_server_attr_init(router));
  ys_server_start(server);

  return EXIT_SUCCESS;
}
```
