#include "server.h"

#include <stdio.h>
#include <string.h>
#include <sys/select.h>

#include "client.h"
#include "config.h"
#include "lib.thread/libthread.h"
#include "libhttp.h"
#include "logger.h"
#include "request.h"
#include "response.h"
#include "router.h"
#include "signal.h"
#include "util.h"
#include "xmalloc.h"

#ifdef USE_TLS
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

typedef struct {
  router_internal *r;
  client_context *c;
} thread_context;

#ifdef USE_TLS
SSL_CTX *create_context() {
  const SSL_METHOD *method = TLS_server_method();
  SSL_CTX *ctx = SSL_CTX_new(method);

  if (!ctx) {
    ERR_print_errors_fp(stderr);
    DIE("failed to create SSL context\n");
  }

  return ctx;
}

void configure_context(SSL_CTX *ctx, const char *certfile,
                       const char *keyfile) {
  if (SSL_CTX_use_certificate_file(ctx, certfile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    DIE("failed to read certificate file %s\n", certfile);
  }

  if (SSL_CTX_use_PrivateKey_file(ctx, keyfile, SSL_FILETYPE_PEM) <= 0) {
    ERR_print_errors_fp(stderr);
    DIE("failed to read private key file %s\n", keyfile);
  }
}
#endif

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
    printlogf(LOG_INFO,
              "[server::%s] error parsing client request with error code %d. "
              "Pre-empting response with internal error handler\n",
              __func__, maybe_req.err.code);

    response_send_error(ctx->c, maybe_req.err.code);

    free(ctx);
    return NULL;
  }

  request_internal *req = maybe_req.req;

  printlogf(LOG_INFO, "[server::%s] client request received: %s %s\n", __func__,
            req->method, req->path);

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
      printlogf(LOG_DEBUG, "[server::%s] select failed; retrying...\n",
                __func__);
    }

    if (FD_ISSET(server_sockfd, &readfds)) {
      if ((client_sockfd = accept(server_sockfd, (struct sockaddr *)&address,
                                  (socklen_t *)&addr_len)) < 0) {
        perror("accept");
        printlogf(LOG_INFO,
                  "[server::%s] failed to accept client socket on %s:%d\n",
                  __func__, address.sin_addr, port);

        continue;
      }

#ifdef USE_TLS
      SSL *ssl = SSL_new(((server_internal *)server)->sslctx);
      SSL_set_fd(ssl, client_sockfd);

      if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);

        response_send_protocol_error(client_sockfd);
        SSL_free(ssl);
        continue;
      }
#endif

      printlogf(LOG_DEBUG,
                "[server::%s] accepted connection from new client; spawning "
                "handler thread...\n",
                __func__);

      client_context *ctx = xmalloc(sizeof(client_context));
      ctx->sockfd = client_sockfd;

#ifdef USE_TLS
      ctx->ssl = ssl;
#endif

      thread_context *tc = xmalloc(sizeof(thread_context));
      tc->c = ctx;
      tc->r = ((server_internal *)server)->router;

      if (!thread_pool_dispatch(pool, client_thread_handler, tc, true)) {
        DIE("[server::%s] failed to dispatch thread from pool\n", __func__);
      }
    }
  }
}

static thread_pool_t *setup_thread_pool() {
  thread_pool_t *pool = calloc(1, sizeof(thread_pool_t));
  if (!pool) {
    DIE("[server::%s] failed to initialized thread pool\n", __func__);
  }

  const unsigned int num_threads = server_conf.threads;
  thread_pool_init(pool);

  for (unsigned int i = 0; i < num_threads; i++) {
    thread_t *client_thread = thread_init(0, "client thread");

    // Make this a detached thread
    thread_set_attr(client_thread, false);
    thread_pool_insert(pool, client_thread);
  }

  return pool;
}

tcp_server *server_init(http_router *router, int port) {
  if (port == -1) {
    port = server_conf.port;
  }

  server_internal *server = xmalloc(sizeof(server_internal));
  server->router = (router_internal *)router;
  server->port = port;

#ifdef USE_TLS
  printlogf(LOG_INFO, "Using TLS - initializing SSL context\n");
  server->sslctx = create_context();
#endif

  return (tcp_server *)server;
}

void server_start(tcp_server *server) {
#ifdef USE_TLS
  server_internal *s = (server_internal *)server;
  printlogf(LOG_INFO, "Loading TLS cert %s and key %s\n", s->cert_path,
            s->key_path);
  configure_context(s->sslctx, s->cert_path, s->key_path);
#endif

  setup_sigint_handler();
  setup_sigsegv_handler();

  thread_pool_t *pool = setup_thread_pool();
  int port = ((server_internal *)server)->port;

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

  memset((char *)&address, NULL_TERM, addr_len);
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

  printlogf(LOG_INFO, "[server::%s] Listening on port %d...\n", __func__, port);

  poll_client_connections(pool, (server_internal *)server, server_sockfd, port,
                          address, addr_len);
}

void server_free(tcp_server *server) {
  router_free((http_router *)((server_internal *)server)->router);
  free(server);
}

void server_set_cert(tcp_server *server, char *certfile, char *keyfile) {
  ((server_internal *)server)->cert_path = certfile;
  ((server_internal *)server)->key_path = keyfile;
}
