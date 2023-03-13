#ifndef REQUEST_H
#define REQUEST_H

/**
 * @brief Request metadata collected from an inbound client request.
 */
typedef struct {
  char *path;
  char *method;
  char *protocol;
  char *host;
  char *user_agent;
  char *accept;
  char *content_len;
  char *content_type;
  char *content;
  char *raw;
} request_t;

#endif /* REQUEST_H */
