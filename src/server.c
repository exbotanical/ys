#include "server.h"

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string.h>
#include <sys/select.h>

#include "client.h"
#include "config.h"
#include "lib.thread/libthread.h"
#include "libys.h"
#include "logger.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include "signal.h"
#include "util.h"
#include "xmalloc.h"

typedef struct {
  router_internal *r;
  client_context *c;
} thread_context;

static SSL_CTX *create_context(void) {
  const SSL_METHOD *method = TLS_server_method();
  SSL_CTX *ctx = SSL_CTX_new(method);

  if (!ctx) {
    ERR_print_errors_fp(stderr);
    DIE("[server::%s] failed to create SSL context\n", __func__);
  }

  return ctx;
}

static void configure_context(SSL_CTX *ctx, const char *certfile,
                              const char *keyfile) {
  if (SSL_CTX_use_certificate_file(ctx, certfile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    DIE("[server::%s] failed to read certificate file %s\n", __func__,
        certfile);
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    DIE("[server::%s] failed to read private key file %s\n", __func__, keyfile);
  }
}

/**
 * client_thread_handler handles client connections and executes the
 * user-defined router
 * @param arg
 * @return void*
 */
static void *client_thread_handler(void *arg) {
  thread_context *ctx = arg;

  maybe_request maybe_req = req_read_and_parse(ctx->c);

  // TODO: test + fix
  if (maybe_req.err.code == IO_ERR || maybe_req.err.code == PARSE_ERR ||
      maybe_req.err.code == REQ_TOO_LONG || maybe_req.err.code == DUP_HDR) {
    printlogf(YS_LOG_INFO,
              "[server::%s] error parsing client request with error code %d. "
              "Pre-empting response with internal error handler\n",
              __func__, maybe_req.err.code);

    response_send_error(ctx->c, maybe_req.err.code);

    free(ctx);
    return NULL;
  }

  request_internal *req = maybe_req.req;

  printlogf(YS_LOG_INFO, "[server::%s] client request received: %s %s\n",
            __func__, req->method, req->path);

  router_run(ctx->r, ctx->c, req);

  free(ctx);
  return NULL;
}

static void poll_client_connections(thread_pool_t *pool,
                                    server_internal *server, int server_sockfd,
                                    int port, struct sockaddr_in address,
                                    ssize_t addr_len) {
  fd_set readfds;
  int client_sockfd;

  while (true) {
    FD_ZERO(&readfds);
    FD_SET(server_sockfd, &readfds);

    if (select(server_sockfd + 1, &readfds, NULL, NULL, NULL) == -1) {
      perror("select");
      printlogf(YS_LOG_DEBUG, "[server::%s] select failed; retrying...\n",
                __func__);
    }

    if (FD_ISSET(server_sockfd, &readfds)) {
      if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&address,
                                  (socklen_t *)&addr_len)) < 0) {
        perror("accept");
        printlogf(YS_LOG_INFO,
                  "[server::%s] failed to accept client socket on %s:%d\n",
                  __func__, address.sin_addr, port);

        continue;
      }

      SSL *ssl = NULL;
      if (server->sslctx) {
        ssl = SSL_new(server->sslctx);
        SSL_set_fd(ssl, client_sockfd);

        if (SSL_accept(ssl) <= 0) {
          ERR_print_errors_fp(stderr);
          printlogf(YS_LOG_INFO,
                    "[server::%s] failed to accept SSL connection\n", __func__);

          response_send_protocol_error(client_sockfd);
          SSL_free(ssl);
          continue;
        }
      }

      printlogf(YS_LOG_DEBUG,
                "[server::%s] accepted connection from new client; spawning "
                "handler thread...\n",
                __func__);

      client_context *ctx = xmalloc(sizeof(client_context));
      ctx->sockfd = client_sockfd;
      ctx->ssl = ssl;

      thread_context *tc = xmalloc(sizeof(thread_context));
      tc->c = ctx;
      tc->r = server->router;

      if (!thread_pool_dispatch(pool, client_thread_handler, tc, true)) {
        DIE("[server::%s] failed to dispatch thread from pool\n", __func__);
      }
    }
  }
}

static thread_pool_t *setup_thread_pool(void) {
  thread_pool_t *pool = calloc(1, sizeof(thread_pool_t));
  if (!pool) {
    DIE("[server::%s] failed to initialized thread pool\n", __func__);
  }

  const unsigned int num_threads = server_conf.threads;
  thread_pool_init(pool);

  for (unsigned int i = 0; i < num_threads; i++) {
    thread_t *client_thread = thread_init(0, "client thread");

    // Make this a detached thread
    // TODO: change threads API to use detached method for clarity
    thread_set_attr(client_thread, false);
    thread_pool_insert(pool, client_thread);
  }

  return pool;
}

tcp_server_attr *server_attr_init(http_router *router) {
  server_attr_internal *attr = xmalloc(sizeof(server_attr_internal));
  attr->router = (router_internal *)router;
  attr->use_https = false;
  attr->port = 0;
  attr->cert_path = NULL;
  attr->key_path = NULL;

  return (tcp_server_attr *)attr;
}

void server_set_port(tcp_server_attr *attr, int port) {
  if (!is_port_in_range(port)) {
    port = server_conf.port;
    printlogf(YS_LOG_INFO, "[server::%s] invalid port %d - defaulting to \n",
              __func__, port, server_conf.port);
  } else {
    ((server_attr_internal *)attr)->port = port;
  }
}

void server_use_https(tcp_server_attr *attr, char *cert_path, char *key_path) {
  bool use_https = cert_path && key_path;
  server_attr_internal *attr_internal = (server_attr_internal *)attr;

  if (use_https) {
    printlogf(YS_LOG_DEBUG, "[server::%s] HTTPs enabled on tcp_server_attr\n",
              __func__);

    attr_internal->use_https = use_https;
    attr_internal->cert_path = cert_path;
    attr_internal->key_path = key_path;
  }
}

void server_disable_https(tcp_server_attr *attr) {
  ((server_attr_internal *)attr)->use_https = false;
}

tcp_server_attr *server_attr_init_with(http_router *router, int port,
                                       char *cert_path, char *key_path) {
  tcp_server_attr *attr = server_attr_init(router);
  server_set_port(attr, port);
  server_use_https(attr, cert_path, key_path);

  return (tcp_server_attr *)attr;
}

tcp_server *server_init(tcp_server_attr *attr) {
  server_attr_internal *a = (server_attr_internal *)attr;

  server_internal *server = xmalloc(sizeof(server_internal));
  server->router = (router_internal *)a->router;
  server->port = a->port ? a->port : server_conf.port;

  if (a->use_https) {
    printlogf(YS_LOG_INFO, "HTTPs enabled - initializing SSL context\n");

    server->key_path = a->key_path;
    server->cert_path = a->cert_path;
    server->sslctx = create_context();
  } else {
    server->sslctx = NULL;
  }

  return (tcp_server *)server;
}

void server_start(tcp_server *server) {
  server_internal *s = (server_internal *)server;

  if (s->sslctx) {
    printlogf(YS_LOG_INFO, "Loading TLS cert %s and key %s\n", s->cert_path,
              s->key_path);
    configure_context(s->sslctx, s->cert_path, s->key_path);
  }

  setup_sigint_handler();
  setup_sigsegv_handler();

  thread_pool_t *pool = setup_thread_pool();
  int port = s->port;

  int server_sockfd;
  if ((server_sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
    perror("socket");
    DIE("[server::%s] failed to initialize server socket on port %d\n",
        __func__, port);
  }

  {
    int yes = 1;
    // avoid the "Address already in use" error message
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                   sizeof(yes)) == -1) {
      perror("setsockopt");
      DIE("[server::%s] failed to set sock opt\n", __func__);
    }
  }

  struct sockaddr_in address;
  socklen_t addr_len = sizeof(address);

  memset((char *)&address, NULL_TERMINATOR, addr_len);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_sockfd, (struct sockaddr *)&address, addr_len) < 0) {
    perror("bind");
    DIE("[server::%s] failed to bind server socket on %s:%d\n", __func__,
        address.sin_addr, port);
  }

  if (listen(server_sockfd, MAX_QUEUED_CONNECTIONS) < 0) {
    perror("listen");
    DIE("[server::%s] failed to listen on %s:%d\n", __func__, address.sin_addr,
        port);
  }

  printlogf(YS_LOG_INFO, "[server::%s] Listening on port %d...\n", __func__,
            port);

  poll_client_connections(pool, s, server_sockfd, port, address, addr_len);
}

void server_free(tcp_server *server) {
  router_free((http_router *)((server_internal *)server)->router);
  free(server);
}
