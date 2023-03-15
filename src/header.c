#include "header.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>  // for malloc
#include <string.h>  // for strlen, strcmp

#include "libhash/libhash.h"
#include "libhttp.h"

static pthread_once_t init_once = PTHREAD_ONCE_INIT;

// common_headers interns common header strings
static const h_table* common_headers;

// to_lower is used to convert chars to lower case
static const int to_lower = 'a' - 'A';

/**
 * init_common_headers initializes the common headers hash table
 */
static void init_common_headers() {
  common_headers = h_init_table(39);

  h_insert(common_headers, "Accept", "Accept");
  h_insert(common_headers, "Accept-Charset", "Accept-Charset");
  h_insert(common_headers, "Accept-Encoding", "Accept-Encoding");
  h_insert(common_headers, "Accept-Language", "Accept-Language");
  h_insert(common_headers, "Accept-Ranges", "Accept-Ranges");
  h_insert(common_headers, "Cache-Control", "Cache-Control");
  h_insert(common_headers, "Cc", "Cc");
  h_insert(common_headers, "Connection", "Connection");
  h_insert(common_headers, "Content-Id", "Content-Id");
  h_insert(common_headers, "Content-Language", "Content-Language");
  h_insert(common_headers, "Content-Length", "Content-Length");
  h_insert(common_headers, "Content-Transfer-Encoding",
           "Content-Transfer-Encoding");
  h_insert(common_headers, "Content-Type", "Content-Type");
  h_insert(common_headers, "Cookie", "Cookie");
  h_insert(common_headers, "Date", "Date");
  h_insert(common_headers, "Dkim-Signature", "Dkim-Signature");
  h_insert(common_headers, "Etag", "Etag");
  h_insert(common_headers, "Expires", "Expires");
  h_insert(common_headers, "From", "From");
  h_insert(common_headers, "Host", "Host");
  h_insert(common_headers, "If-Modified-Since", "If-Modified-Since");
  h_insert(common_headers, "If-None-Match", "If-None-Match");
  h_insert(common_headers, "In-Reply-To", "In-Reply-To");
  h_insert(common_headers, "Last-Modified", "Last-Modified");
  h_insert(common_headers, "Location", "Location");
  h_insert(common_headers, "Message-Id", "Message-Id");
  h_insert(common_headers, "Mime-Version", "Mime-Version");
  h_insert(common_headers, "Pragma", "Pragma");
  h_insert(common_headers, "Received", "Received");
  h_insert(common_headers, "Return-Path", "Return-Path");
  h_insert(common_headers, "Server", "Server");
  h_insert(common_headers, "Set-Cookie", "Set-Cookie");
  h_insert(common_headers, "Subject", "Subject");
  h_insert(common_headers, "To", "To");
  h_insert(common_headers, "User-Agent", "User-Agent");
  h_insert(common_headers, "Via", "Via");
  h_insert(common_headers, "X-Forwarded-For", "X-Forwarded-For");
  h_insert(common_headers, "X-Imforwards", "X-Imforwards");
  h_insert(common_headers, "X-Powered-By", "X-Powered-By");
}

/**
 * get_common_header_t atomically initializes and retrieves the common headers
 * hash table. Initialization is guaranteed to only run once, on the first
 * invocation
 *
 * @return h_table*
 */
static h_table* get_common_header_t() {
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

  h_table* common_headers = get_common_header_t();

  h_record* result = h_search(common_headers, s);
  if (result) {
    return result->value;
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
  header_t* h = (header_t*)array_find(headers, match_header, key);

  return h ? h->value : NULL;
}

void append_header(array_t* headers, char* key, char* value) {
  header_t* h = malloc(sizeof(header_t));

  h->key = key;
  h->value = value;
  array_push(headers, h);
}
