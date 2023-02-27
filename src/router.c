#include "router.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "server.h"

route_context_t *route_context_init(int client_socket, char *path, char *method,
                                    char *protocol, char *host,
                                    char *user_agent, char *accept,
                                    char *content_len, char *content_type,
                                    char *content, char *raw,
                                    Array *parameters) {
  route_context_t *context = malloc(sizeof(route_context_t));
  if (!context) {
    free(context);

    LOG("[context::route_context_init] %s\n",
        "failed to allocate route_context_t");

    return NULL;
  }

  context->client_socket = client_socket;
  context->path = path;
  context->method = method;
  context->protocol = protocol;
  context->host = host;
  context->user_agent = user_agent;
  context->accept = accept;
  context->content_len = content_len;
  context->content_type = content_type;
  context->content = content;
  context->raw = raw;
  context->parameters = parameters;

  return context;
}

router_t *router_init(void *(*not_found_handler)(void *),
                      void *(*method_not_allowed_handler)(void *)) {
  router_t *router = malloc(sizeof(router_t));
  if (!router) {
    router_free(router);
    LOG("[router::router_init] %s\n", "failed to allocate router_t");

    return NULL;
  }

  router->trie = trie_init();
  if (!router->trie) {
    router_free(router);
    LOG("[router::router_init] %s\n", "failed to allocate trie via trie_init");

    return NULL;
  }

  if (!not_found_handler) {
    LOG("[router::router_init] %s\n",
        "not_found_handler is NULL, registering fallback handler");
    router->not_found_handler = default_not_found_handler;
  } else {
    router->not_found_handler = not_found_handler;
  }

  if (!method_not_allowed_handler) {
    LOG("[router::router_init] %s\n",
        "method_not_allowed_handler is NULL, registering fallback handler");
    router->method_not_allowed_handler = default_method_not_allowed_handler;
  } else {
    router->method_not_allowed_handler = method_not_allowed_handler;
  }

  return router;
}

bool router_register(router_t *router, ch_array_t *methods, const char *path,
                     void *(*handler)(void *)) {
  if (!router || !methods || !path || !handler) {
    DIE(EXIT_FAILURE, "%s\n",
        "invariant violation - router_register arguments cannot be NULL");
  }

  if (!trie_insert(router->trie, methods, path, handler)) {
    char *message = "failed to insert route record";
    LOG("[route::router_register] %s\n", message);
    STDERR("%s\n", message);

    return false;
  }

  return true;
}

void router_run(router_t *router, route_context_t *context) {
  if (!context) {
    LOG("[router::router_run] context initialization via route_context_init \
			failed with method %s, path %s",
        context->method, context->path);

    return;
  }

  response_t *response;
  result_t *result = trie_search(router->trie, context->method, context->path);

  if (!result) {
    response = internal_server_error_handler(context);
  } else if ((result->flags & NOT_FOUND_MASK) == NOT_FOUND_MASK) {
    response = router->not_found_handler(context);
  } else if ((result->flags & NOT_ALLOWED_MASK) == NOT_ALLOWED_MASK) {
    response = router->method_not_allowed_handler(context);
  } else {
    context->parameters = result->parameters;
    response_t *(*h)(void *) = result->action->handler;
    response = h(context);
  }

  send_response(context->client_socket, response);
}

void router_free(router_t *router) { free(router); }

route_t *route_init(ch_array_t *methods, char *path, void *(*handler)(void *)) {
  route_t *route_record = malloc(sizeof(route_t));
  if (!route_record) {
    free(route_record);
    LOG("[router::route_init] %s\n", "failed to allocate route_record");

    return NULL;
  }

  route_record->methods = methods;
  route_record->path = path;
  route_record->handler = handler;

  return route_record;
}

ch_array_t *collect_methods(char *method, ...) {
  ch_array_t *methods = ch_array_init();
  if (!methods) {
    LOG("[router::collect_methods] %s\n",
        "failed to allocate methods array via ch_array_init");

    return NULL;
  }

  va_list args;
  va_start(args, method);

  while (method) {
    if (!ch_array_insert(methods, method)) {
      free(methods);
      LOG("[router::collect_methods] %s\n",
          "failed to insert into methods array");

      return NULL;
    }

    method = va_arg(args, char *);
  }

  va_end(args);
  return methods;
}

void *internal_server_error_handler(void *arg) {
  route_context_t *context = arg;
  LOG("[router::internal_server_error_handler] 500 handler in effect at "
      "request path %s\n",
      context->path);

  response_t *response = response_init();
  response->status = INTERNAL_SERVER_ERROR;

  return response;
}

void *default_not_found_handler(void *arg) {
  route_context_t *context = arg;
  LOG("[router::default_not_found_handler] default 404 handler in effect at "
      "request path %s\n",
      context->path);

  response_t *response = response_init();
  response->status = NOT_FOUND;

  return response;
}

void *default_method_not_allowed_handler(void *arg) {
  route_context_t *context = arg;
  LOG("[router::default_method_not_allowed_handler] default 405 handler in "
      "effect at request path %s\n",
      context->path);

  response_t *response = response_init();
  response->status = METHOD_NOT_ALLOWED;

  return response;
}
