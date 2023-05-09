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

response *index_handler(request *req, response *res) {
  set_status(res, STATUS_OK);
  set_header(res, "Content-Type", MIME_TYPE_HTML);
  set_body(res, from_file("./index.html"));

  return res;
}

response *css_handler(request *req, response *res) {
  set_status(res, STATUS_OK);
  set_header(res, "Content-Type", MIME_TYPE_CSS);
  set_body(res, from_file("./style.css"));

  return res;
}

response *data_handler(request *req, response *res) {
  cookie *c = get_cookie(req, COOKIE_ID);
  char *sid = cookie_get_value(c);

  pthread_mutex_lock(&session_mutex);
  char *username = ht_get(session_store, sid);
  pthread_mutex_unlock(&session_mutex);

  set_body(res, "{ \"data\": \"Hello, %s!\" }", username);
  set_header(res, "Content-Type", MIME_TYPE_JSON);
  set_status(res, STATUS_OK);

  return res;
}

response *register_handler(request *req, response *res) {
  char *username = jsob_getstr(req_get_body(req), "username");
  if (!username) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "Must provide a username");
    return res;
  }

  char *password = jsob_getstr(req_get_body(req), "password");
  if (!password) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "Must provide a password");
    return res;
  }

  if (user_exists(username)) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "Username %s exists", username);
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

  cookie *c = cookie_init(COOKIE_ID, sid);
  cookie_set_expires(c, n_minutes_from_now(SESSION_TIMEOUT_MINUTES));

  set_cookie(res, c);

  set_status(res, STATUS_CREATED);
  return res;
}

response *login_handler(request *req, response *res) {
  char *username = jsob_getstr(req_get_body(req), "username");
  if (!username) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "Must provide a username");
    return res;
  }

  char *password = jsob_getstr(req_get_body(req), "password");
  if (!password) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "Must provide a password");
    return res;
  }

  if (!user_exists(username)) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "invalid credentials");

    return res;
  }

  pthread_mutex_lock(&user_mutex);
  user *u = (char *)ht_get(user_store, username);
  pthread_mutex_unlock(&user_mutex);

  if (!u || !s_equals(hash_password(u->password), password)) {
    set_status(res, STATUS_BAD_REQUEST);
    set_body(res, "invalid credentials");

    return res;
  }

  char *session_id = create_sid();

  pthread_mutex_lock(&session_mutex);
  ht_insert(session_store, session_id, username);
  pthread_mutex_unlock(&session_mutex);

  cookie *c = cookie_init(COOKIE_ID, session_id);
  cookie_set_expires(c, n_minutes_from_now(SESSION_TIMEOUT_MINUTES));
  cookie_set_path(c, "/");
  set_cookie(res, c);

  set_status(res, STATUS_OK);

  return res;
}

response *logout_handler(request *req, response *res) {
  cookie *c = get_cookie(req, COOKIE_ID);
  if (!c) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res);

    return res;
  }

  char *sid = cookie_get_value(c);
  if (!sid) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res);

    return res;
  }

  pthread_mutex_lock(&session_mutex);
  ht_delete(session_store, sid);
  pthread_mutex_unlock(&session_mutex);

  cookie_set_max_age(c, -1);
  set_cookie(res, c);

  return res;
}

response *auth_middleware(request *req, response *res) {
  set_header(res, "X-Authorized-By", "TheDemoApp");

  cookie *c = get_cookie(req, COOKIE_ID);
  if (!c) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res);

    return res;
  }

  char *sid = cookie_get_value(c);

  pthread_mutex_lock(&session_mutex);
  char *username = ht_get(session_store, sid);
  pthread_mutex_unlock(&session_mutex);

  if (!username || !user_exists(username)) {
    set_status(res, STATUS_UNAUTHORIZED);
    set_done(res);

    return res;
  }

  cookie_set_expires(c, n_minutes_from_now(SESSION_TIMEOUT_MINUTES));
  set_cookie(res, c);

  return res;
}

int main() {
  session_store = ht_init(0);
  user_store = ht_init(0);

  router_attr *attr = router_attr_init();
  add_middleware_with_opts(attr, auth_middleware, "^/$", "^/style.css$",
                           "^/login$", "^/register$");

  http_router *router = router_init(attr);

  router_register(router, "/register", register_handler, METHOD_POST);
  router_register(router, "/login", login_handler, METHOD_POST);
  router_register(router, "/logout", logout_handler, METHOD_POST);
  router_register(router, "/data", data_handler, METHOD_GET);

  router_register(router, "/", index_handler, METHOD_GET);
  router_register(router, "/style.css", css_handler, METHOD_GET);

  tcp_server *server = server_init(server_attr_init(router));
  server_start(server);

  return EXIT_SUCCESS;
}
