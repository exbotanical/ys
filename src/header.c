#include "header.h"

#include <ctype.h>    // for tolower
#include <pthread.h>  // for pthread_once
#include <stdbool.h>
#include <string.h>  // for strlen

#include "libhttp.h"
#include "libutil/libutil.h"  // for s_equals
#include "util.h"
#include "xmalloc.h"

static pthread_once_t init_common_headers_once = PTHREAD_ONCE_INIT;
static pthread_once_t init_singleton_headers_once = PTHREAD_ONCE_INIT;

// common_headers interns common header strings
static hash_set* common_headers;

// singleton_headers interns common header strings
static hash_set* singleton_headers;

// to_lower is used to convert chars to lower case
static const int to_lower = 'a' - 'A';

/**
 * init_common_headers initializes the common headers hash set
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
 * init_singleton_headers initializes the singleton headers hash set. A
 * singleton header is a header that cannot be duplicated for a request per
 * RFC 7230.
 * TODO: test
 * TODO: case sensitive?
 */
static void init_singleton_headers() {
  singleton_headers = hs_init(3);

  hs_insert(singleton_headers, "Content-Type");
  hs_insert(singleton_headers, "Content-Length");
  hs_insert(singleton_headers, "Host");
}

/**
 * get_common_header_set atomically initializes and retrieves the common headers
 * hash set. Initialization is guaranteed to only run once, on the first
 * invocation
 *
 * @return hash_set*
 */
static hash_set* get_common_header_set() {
  pthread_once(&init_common_headers_once, init_common_headers);

  return common_headers;
}

/**
 * get_singleton_header_set atomically initializes and retrieves the singleton
 * headers hash set. Initialization is guaranteed to only run once, on the first
 * invocation
 *
 * @return hash_set*
 */
static hash_set* get_singleton_header_set() {
  pthread_once(&init_singleton_headers_once, init_singleton_headers);

  return singleton_headers;
}

/**
 * is_singleton_header returns a bool indicating whether the given header
 * `key` is a singleton header
 *
 * @param key
 * @return bool
 */
static bool is_singleton_header(const char* key) {
  return hs_contains(get_singleton_header_set(), key);
}

/**
 * canonical_mime_header_key modifies a header key into a valid format
 *
 * @param s
 * @return char*
 */
static char* canonical_mime_header_key(char* key) {
  hash_set* common_headers = get_common_header_set();
  // Avoid needless computation if we know the header is already correct
  if (hs_contains(common_headers, key)) {
    return key;
  }

  // See if it looks like a header key; if not return it as-is
  for (unsigned int i = 0; i < strlen(key); i++) {
    char c = key[i];
    if (is_valid_header_field_char(c)) {
      continue;
    }

    return key;
  }

  bool upper = true;
  for (unsigned int i = 0; i < strlen(key); i++) {
    char c = key[i];
    // Canonicalize: first letter upper case and upper case after each dash
    // MIME headers are ASCII only, so no Unicode issues
    if (upper && 'a' <= c && c <= 'z') {
      c -= to_lower;
    } else if (!upper && 'A' <= c && c <= 'Z') {
      c += to_lower;
    }

    // TODO: fix ... ugh this is disgusting
    unsigned int l = strlen(key);
    char* ca = xmalloc(l + 1);
    strncpy(ca, key, l);
    ca[l] = '\0';
    ca[i] = c;
    key = ca;

    upper = c == '-';  // for next time
  }

  return key;
}

bool is_valid_header_field_char(int c) {
  return c < sizeof(token_table) && token_table[c];
}

char* to_canonical_mime_header_key(char* key) {
  // Check for canonical encoding
  bool upper = true;
  for (unsigned int i = 0; i < strlen(key); i++) {
    char c = key[i];
    if (!is_valid_header_field_char(c)) {
      return key;
    }

    if (upper && 'a' <= c && c <= 'z') {
      return canonical_mime_header_key(key);
    }

    if (!upper && 'A' <= c && c <= 'Z') {
      return canonical_mime_header_key(key);
    }

    upper = c == '-';
  }

  return key;
}

char* get_header(hash_table* headers, const char* key) {
  ht_record* header = ht_search(headers, key);
  if (!header) {
    return NULL;
  }

  return array_get(header->value, 0);
}

char** req_header_values(hash_table* headers, const char* key) {
  ht_record* header = ht_search(headers, key);
  if (!header) {
    return NULL;
  }

  unsigned int sz = array_size(header->value);
  char** headers_list = xmalloc(sz);

  for (unsigned int i = 0; i < sz; i++) {
    headers_list[i] = (char*)array_get(header->value, i);
  }

  return headers_list;
}

bool set_header(response* res, const char* key, const char* value) {
  return insert_header(res->headers, key, value);  // TODO: singleton check
}

// TODO: should check singleton flag so we can use w/response
// TODO: does get_header handle header=value1;value2 ?
bool insert_header(hash_table* headers, const char* key, const char* value) {
  // Check if we've already encountered this header key. Some headers cannot
  // be duplicated e.g. Content-Type, so we'll need to handle those as well
  ht_record* existing_headers = ht_search(headers, key);
  if (existing_headers) {
    // Disallow duplicates where necessary e.g. multiple Content-Type headers is
    // a 400 This follows Section 4.2 of RFC 7230 to ensure we handle multiples
    // of the same header correctly
    if (is_singleton_header(key)) {
      return false;
    }

    array_push(existing_headers->value, (void*)value);
  } else {
    // We haven't encountered this header before; insert into the hash table
    // along with the first value
    array_t* values = array_init();
    if (!values) {
      DIE(EXIT_FAILURE, "[header::%s] failed to allocate array\n", __func__);
    }

    array_push(values, (void*)value);
    ht_insert(headers, key, values);
  }

  return true;
}

array_t* derive_headers(const char* header_str) {
  array_t* headers = array_init();

  if (s_nullish(header_str)) {
    return headers;
  }

  unsigned int len = strlen(header_str);
  array_t* tmp = array_init();

  for (unsigned int i = 0; i < len; i++) {
    char c = header_str[i];

    if ((c >= 'a' && c <= 'z') || c == '_' || c == '-' || c == '.' ||
        (c >= '0' && c <= '9')) {
      array_push(tmp, c);
    }

    if (c >= 'A' && c <= 'Z') {
      array_push(tmp, tolower(c));
    }

    if (c == ' ' || c == ',' || i == len - 1) {
      unsigned int size = array_size(tmp);
      if (size > 0) {
        char* v = xmalloc(size + 1);
        unsigned int i;
        for (i = 0; i < size; i++) {
          v[i] = (char)array_get(tmp, i);
        }

        v[i + 1] = '\0';

        array_push(headers, v);

        array_free(tmp);
        tmp = array_init();
      }
    }
  }

  return headers;
}
