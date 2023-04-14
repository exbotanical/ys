#ifndef CORS_H
#define CORS_H

#include "libutil/libutil.h"
#include "libys.h"
#include "request.h"

/**
 * CORS configuration options
 * @internal
 */
typedef struct {
  bool allow_credentials;
  bool use_options_passthrough;
  unsigned int max_age;
  array_t *allowed_origins;
  array_t *allowed_methods;
  array_t *allowed_headers;
  array_t *expose_headers;
} cors_opts_internal;

/**
 * CORS configurations
 */
typedef struct {
  unsigned int max_age;
  bool allow_credentials;
  bool allow_all_headers;
  bool use_options_passthrough;
  bool allow_all_origins;
  array_t *allowed_methods;
  array_t *allowed_origins;
  array_t *allowed_headers;
  array_t *exposed_headers;
} cors_config;

/**
 * cors_init initializes a CORS context using the user-specified settings on the
 * given cors_opts_internal `opts`
 */
cors_config *cors_init(cors_opts_internal *opts);

/**
 * cors_handler is a middleware handler that executes a spec-compliant CORS
 * workflow
 */
response *cors_handler(request *req, response *res);

#endif /* CORS_H */
