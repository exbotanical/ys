#include "header.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>  // for malloc
#include <string.h>  // for strlen, strcmp

#include "libhash/libhash.h"
#include "libhttp.h"

static pthread_once_t init_once = PTHREAD_ONCE_INIT;

// commonHeader interns common header strings.
static const h_table* common_headers;

static const int to_lower = 'a' - 'A';

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

static h_table* get_common_header_t() {
  pthread_once(&init_once, init_common_headers);

  return common_headers;
}

static bool valid_header_field_byte(int b) {
  return b < sizeof(token_table) && token_table[b];
}

// canonicalMIMEHeaderKey is like CanonicalMIMEHeaderKey but is
// allowed to mutate the provided byte slice before returning the
// string.
//
// For invalid inputs (if a contains spaces or non-token bytes), a
// is unchanged and a string copy is returned.
static char* canonical_mime_header_key(char* s) {
  // See if a looks like a header key. If not, return it unchanged.
  for (unsigned int i = 0; i < strlen(s); i++) {
    char c = s[i];
    if (valid_header_field_byte(c)) {
      continue;
    }
    // Don't canonicalize.
    return s;
  }

  bool upper = true;
  for (unsigned int i = 0; i < strlen(s); i++) {
    char c = s[i];
    // Canonicalize: first letter upper case
    // and upper case after each dash.
    // (Host, User-Agent, If-Modified-Since).
    // MIME headers are ASCII only, so no Unicode issues.
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

  // The compiler recognizes m[string(byteSlice)] as a special
  // case, so a copy of a's bytes into a new string does not
  // happen in this map lookup:

  h_record* result = h_search(common_headers, s);
  if (result) {
    return result->value;
  }

  return s;
}

// CanonicalMIMEHeaderKey returns the canonical format of the
// MIME header key s. The canonicalization converts the first
// letter and any letter following a hyphen to upper case;
// the rest are converted to lowercase. For example, the
// canonical key for "accept-encoding" is "Accept-Encoding".
// MIME header keys are assumed to be ASCII only.
// If s contains a space or invalid header field bytes, it is
// returned without modifications.
char* to_canonical_MIME_header_key(char* s) {
  // Quick check for canonical encoding.
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

// TODO:
