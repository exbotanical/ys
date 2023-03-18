#include "header.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>  // for malloc
#include <string.h>  // for strlen, strcmp

#include "libhash/libhash.h"  // for hash sets
#include "libhttp.h"

static pthread_once_t init_once = PTHREAD_ONCE_INIT;

// common_headers interns common header strings
static const hash_set* common_headers;

// to_lower is used to convert chars to lower case
static const int to_lower = 'a' - 'A';

/**
 * init_common_headers initializes the common headers hash table
 */
static void init_common_headers() {
  common_headers = hs_init(39);

  hs_insert(common_headers, "Accept");
  hs_insert(common_headers, "Accept-Charset");
  hs_insert(common_headers, "Accept-Encoding");
  hs_insert(common_headers, "Accept-Language");
  hs_insert(common_headers, "Accept-Ranges");
  hs_insert(common_headers, "Cache-Control");
  hs_insert(common_headers, "Cc");
  hs_insert(common_headers, "Connection");
  hs_insert(common_headers, "Content-Id");
  hs_insert(common_headers, "Content-Language");
  hs_insert(common_headers, "Content-Length");
  hs_insert(common_headers, "Content-Transfer-Encoding");
  hs_insert(common_headers, "Content-Type");
  hs_insert(common_headers, "Cookie");
  hs_insert(common_headers, "Date");
  hs_insert(common_headers, "Dkim-Signature");
  hs_insert(common_headers, "Etag");
  hs_insert(common_headers, "Expires");
  hs_insert(common_headers, "From");
  hs_insert(common_headers, "Host");
  hs_insert(common_headers, "If-Modified-Since");
  hs_insert(common_headers, "If-None-Match");
  hs_insert(common_headers, "In-Reply-To");
  hs_insert(common_headers, "Last-Modified");
  hs_insert(common_headers, "Location");
  hs_insert(common_headers, "Message-Id");
  hs_insert(common_headers, "Mime-Version");
  hs_insert(common_headers, "Pragma");
  hs_insert(common_headers, "Received");
  hs_insert(common_headers, "Return-Path");
  hs_insert(common_headers, "Server");
  hs_insert(common_headers, "Set-Cookie");
  hs_insert(common_headers, "Subject");
  hs_insert(common_headers, "To");
  hs_insert(common_headers, "User-Agent");
  hs_insert(common_headers, "Via");
  hs_insert(common_headers, "X-Forwarded-For");
  hs_insert(common_headers, "X-Imforwards");
  hs_insert(common_headers, "X-Powered-By");
}

/**
 * get_common_header_set atomically initializes and retrieves the common headers
 * hash table. Initialization is guaranteed to only run once, on the first
 * invocation
 *
 * @return hash_set*
 */
static hash_set* get_common_header_set() {
  pthread_once(&init_once, init_common_headers);

  return common_headers;
}

/**
 * valid_header_field_byte returns a bool indicating whether the given byte is a
 * valid header char
 *
 * @param b
 * @return bool
 */
static bool valid_header_field_byte(int b) {
  return b < sizeof(token_table) && token_table[b];
}

/**
 * canonical_mime_header_key modifies a header key into a valid format
 *
 * @param s
 * @return char*
 */
static char* canonical_mime_header_key(char* s) {
  hash_set* common_headers = get_common_header_set();
  // Avoid needless computation if we know the header is already correct
  if (hs_contains(common_headers, s)) {
    return s;
  }

  // See if it looks like a header key; if not return it as-is
  for (unsigned int i = 0; i < strlen(s); i++) {
    char c = s[i];
    if (valid_header_field_byte(c)) {
      continue;
    }

    return s;
  }

  bool upper = true;
  for (unsigned int i = 0; i < strlen(s); i++) {
    char c = s[i];
    // Canonicalize: first letter upper case and upper case after each dash
    // MIME headers are ASCII only, so no Unicode issues
    if (upper && 'a' <= c && c <= 'z') {
      c -= to_lower;
    } else if (!upper && 'A' <= c && c <= 'Z') {
      c += to_lower;
    }

    // TODO: fix ... ugh this is disgusting
    char* ca = malloc(strlen(s) + 1);
    strncpy(ca, s, strlen(s));
    ca[strlen(s)] = '\0';
    ca[i] = c;
    s = ca;

    upper = c == '-';  // for next time
  }

  return s;
}

char* to_canonical_MIME_header_key(char* s) {
  // Check for canonical encoding
  bool upper = true;
  for (unsigned int i = 0; i < strlen(s); i++) {
    char c = s[i];
    if (!valid_header_field_byte(c)) {
      return s;
    }

    if (upper && 'a' <= c && c <= 'z') {
      return canonical_mime_header_key(s);
    }

    if (!upper && 'A' <= c && c <= 'Z') {
      return canonical_mime_header_key(s);
    }

    upper = c == '-';
  }

  return s;
}

/**
 * match_header is a matcher function for header comparisons
 *
 * @param h
 * @param s
 * @return bool
 */
bool match_header(header_t* h, char* s) { return strcmp(h->key, s) == 0; }

char* get_header_value(array_t* headers, char* key) {
  int idx = array_find(headers, match_header, key);
  if (idx == -1) {
    return NULL;
  }

  header_t* h = array_get(headers, idx);
  return h->value;
}

void append_header(array_t* headers, char* key, char* value) {
  header_t* h = malloc(sizeof(header_t));

  h->key = key;
  h->value = value;
  array_push(headers, h);
}
