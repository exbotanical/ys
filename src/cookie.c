#define __USE_XOPEN  // See `man 7 feature_test_macros`
#define _XOPEN_SOURCE
#define _GNU_SOURCE

#include "cookie.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "header.h"
#include "ip.h"
#include "libhash/libhash.h"
#include "libutil/libutil.h"
#include "logger.h"
#include "request.h"
#include "util.h"
#include "xmalloc.h"

#define COOKIE_TIME_LEN 30

static const char COOKIE_TIME_FMT[] = "%a, %d %b %Y %H:%M:%S GMT";

static const char SET_COOKIE[] = "Set-Cookie";
static const char COOKIE[] = "Cookie";

/**
 * sanitize cleans and formats the cookie field
 */
static char* sanitize(const char* field_name, bool (*test)(const char),
                      const char* v) {
  bool ok = true;
  for (unsigned int i = 0; i < strlen(v); i++) {
    if (test(v[i])) {
      continue;
    }

    printlogf(YS_LOG_DEBUG,
              "invalid char %c in %s; dropping invalid characters", v[i],
              field_name);
    ok = false;
    break;
  }

  if (ok) {
    return s_copy(v);
  }

  buffer_t* b = buffer_init(NULL);
  for (unsigned int i = 0; i < strlen(v); i++) {
    char c = v[i];

    if (test(c)) {
      buffer_append(b, tostr(c));
    }
  }

  return buffer_state(b);
}

/**
 * is_valid_cookie_value_char tests whether a char c is a valid cookie value
 * character. Returns a bool indicating whether c is ASCII but not " OR ; OR \
 */
static bool is_valid_cookie_value_char(char c) {
  return is_ascii(c) && c != '"' && c != ';' && c != '\\';
}

/**
 * is_valid_cookie_path_char tests whether a char c is a valid cookie path
 * character. Returns a bool bool indicating whether c is ASCII but not ;
 */
static bool is_valid_cookie_path_char(char c) {
  return is_ascii(c) && c != ';';
}

/**
 * is_valid_cookie_name tests whether the given string is a valid cookie name.
 * Returns a bool indicating whether name is non-NULL, non-empty, and contains
 * only valid header field chars
 */
static bool is_valid_cookie_name(const char* name) {
  if (s_nullish(name)) {
    return false;
  }

  for (unsigned int i = 0; i < strlen(name); i++) {
    char c = name[i];
    if (is_valid_header_field_char(c)) {
      continue;
    }
    return false;
  }

  return true;
}

/**
 * is_valid_cookie_domain_name tests whether s is a valid domain name or a valid
 * domain name with a leading dot '.'
 */
static bool is_valid_cookie_domain_name(const char* s) {
  if (strlen(s) == 0) {
    return false;
  }

  if (strlen(s) > 255) {
    return false;
  }

  char* cp = s_copy(s);

  if (cp[0] == '.') {
    // A cookie a domain attribute may start with a leading dot
    strcpy(cp, s_substr(cp, 1, strlen(cp), false));
  }

  // The previous character
  char prev = '.';
  // true once we've seen a letter
  bool ok = false;
  // Retval
  bool ret = false;
  // The length of the current domain component we're parsing
  int component_len = 0;

  for (unsigned int i = 0; i < strlen(cp); i++) {
    char c = cp[i];
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) {
      ok = true;
      component_len++;
    } else if ('0' <= c && c <= '9') {
      component_len++;
    } else if (c == '-') {
      // Character preceding a dash cannot be dot
      if (prev == '.') {
        goto done;
      }

      component_len++;
    } else if (c == '.') {
      // Character preceding a dot cannot be another dot or dash
      if (prev == '.' || prev == '-') {
        goto done;
      }

      if (component_len > 63 || component_len == 0) {
        goto done;
      }

      component_len = 0;
    } else {
      goto done;
    }

    prev = c;
  }

  if (prev == '-' || component_len > 63) {
    goto done;
  }

  ret = ok;
  goto done;

done:
  free(cp);
  return ret;
}

/**
 * is_valid_cookie_domain tests whether string d is a valid cookie domain or IP
 * address
 */
static bool is_valid_cookie_domain(const char* d) {
  if (is_valid_cookie_domain_name(d)) {
    return true;
  }

  if (validate_ip(d) && !strstr(d, ":")) {
    return true;
  }

  return false;
}

/**
 * is_valid_cookie_expiry tests whether t is a valid cookie expiry. Valid here
 * means t's year is greater than 1601 and t itself is not less than or equal to
 * zero.
 */
static bool is_valid_cookie_expiry(time_t t) {
  if (t <= 0) return false;
  // IETF RFC 6265, Section 5.1.1.5 - Abort these steps and fail to parse the
  // cookie-date if ... the year-value is less than 1601
  struct tm* timeinfo = localtime(&t);
  return timeinfo->tm_year + 1900 >= 1601;
}

/**
 * parse_cookie_value parses the raw cookie string to extract a value
 */
static char* parse_cookie_value(const char* raw, bool allow_dbl_quote) {
  if (raw == NULL) {
    // Some attributes like HttpOnly may have an empty value
    return "";
  }

  char* cp = s_copy(raw);

  // Strip the quotes, if present
  if (allow_dbl_quote && strlen(cp) > 1 && cp[0] == '"' &&
      cp[strlen(cp) - 1] == '"') {
    strcpy(cp, s_substr(cp, 1, strlen(cp) - 1, false));
  }

  for (unsigned int i = 0; i < strlen(cp); i++) {
    if (!is_valid_cookie_value_char(cp[i])) {
      return NULL;
    }
  }

  return cp;
}

/**
 * sanitize_cookie_value sanitizes and cleans the cookie value
 */
static char* sanitize_cookie_value(char* v) {
  v = sanitize("Cookie.Value", is_valid_cookie_value_char, v);

  if (strlen(v) == 0) {
    return v;
  }

  if (strstr(v, ",") || strstr(v, " ")) {
    char* ret = fmt_str("\"%s\"", v);
    free(v);
    return ret;
  }

  return v;
}

/**
 * sanitize_cookie_path sanitizes and cleans the cookie path
 */
static char* sanitize_cookie_path(const char* p) {
  return sanitize("Cookie.Path", is_valid_cookie_path_char, p);
}

/**
 * cookie_serialize returns the stringified ys_cookie* for use in a Cookie
 * header, or NULL if the cookie name is invalid
 */
static char* cookie_serialize(cookie_internal* c) {
  if (c == NULL || !is_valid_cookie_name(c->name)) {
    return NULL;
  }

  buffer_t* b = buffer_init(NULL);

  buffer_append(b, c->name);
  buffer_append(b, "=");
  buffer_append(b, sanitize_cookie_value(c->value));

  if (!s_nullish(c->path) && strlen(c->path) > 0) {
    buffer_append(b, "; Path=");
    buffer_append(b, sanitize_cookie_path(c->path));
  }

  if (!s_nullish(c->domain) && strlen(c->domain) > 0) {
    if (is_valid_cookie_domain(c->domain)) {
      // A domain containing illegal characters is not sanitized but simply
      // dropped which turns the cookie into a host-only cookie. A leading dot
      // is acceptable but will be truncated off of the result
      char* d = c->domain;
      if (d[0] == '.') {
        d = s_substr(d, 1, strlen(d), false);
      }

      buffer_append(b, "; Domain=");
      buffer_append(b, d);

    } else {
      printlogf(YS_LOG_DEBUG,
                "invalid cookie domain %s; dropping domain attribute",
                c->domain);
    }
  }

  if (is_valid_cookie_expiry(c->expires)) {
    buffer_append(b, "; Expires=");

    char expires_str[COOKIE_TIME_LEN];

    struct tm* tm_info = gmtime(&c->expires);
    strftime(expires_str, COOKIE_TIME_LEN, COOKIE_TIME_FMT, tm_info);

    buffer_append(b, expires_str);
  }

  if (c->max_age > 0) {
    buffer_append(b, "; Max-Age=");
    buffer_append(b, safe_itoa(c->max_age));
  } else if (c->max_age < 0) {
    buffer_append(b, "; Max-Age=0");
  }

  if (c->http_only) {
    buffer_append(b, "; HttpOnly");
  }

  if (c->secure) {
    buffer_append(b, "; Secure");
  }

  switch (c->same_site) {
    case SAME_SITE_DEFAULT_MODE:
      break;

    case SAME_SITE_NONE_MODE:
      buffer_append(b, "; SameSite=None");
      break;

    case SAME_SITE_LAX_MODE:
      buffer_append(b, "; SameSite=Lax");
      break;

    case SAME_SITE_STRICT_MODE:
      buffer_append(b, "; SameSite=Strict");
      break;
  }

  return buffer_state(b);
}

static array_t* read_browser_cookie(hash_table* headers) {
  array_t* cookies = array_init();

  array_t* lines = (array_t*)ht_get(headers, COOKIE);
  if (!has_elements(lines)) {
    return cookies;
  }

  foreach (lines, i) {
    char* line = s_trim(array_get(lines, i));

    while (!s_nullish(line)) {
      array_t* split = str_cut(line, ";");

      char* part = array_get(split, 0);
      line = array_get(split, 1);
      array_free(split);

      if (s_nullish(part)) {
        continue;
      }

      array_t* pair = str_cut(part, "=");
      char* name = array_get(pair, 0);
      char* value = array_get(pair, 1);
      array_free(pair);

      if (!is_valid_cookie_name(name)) {
        continue;
      }

      value = parse_cookie_value(value, true);
      if (s_nullish(value)) {
        continue;
      }

      array_push(cookies, ys_cookie_init(name, value));
    }
    free(line);
  }

  return cookies;
}

/**
 * read_cookies parses all "Cookie" values from the given headers and returns
 * the successfully parsed cookies
 */
static array_t* read_cookies(hash_table* headers) {
  array_t* cookies = (array_t*)ht_get(headers, COOKIE);

  array_t* ret = array_init();

  if (!has_elements(cookies)) {
    return ret;
  }

  foreach (cookies, i) {
    char* cstr = array_get(cookies, i);

    array_t* parts = split(s_trim(cstr), ";");
    if (array_size(parts) == 0) {
      array_free(parts);
      return read_browser_cookie(headers);  // TODO: test
    }

    if (array_size(parts) == 1 && s_nullish(array_get(parts, 0))) {
      array_free(parts);
      continue;
    }

    ((__array_t*)parts)->state[0] = s_trim(((__array_t*)parts)->state[0]);

    array_t* sub_parts = str_cut(array_get(parts, 0), "=");

    if (array_size(sub_parts) == 0) {
      array_free(parts);
      array_free(sub_parts);
      continue;
    }

    char* name = array_get(sub_parts, 0);
    if (!is_valid_cookie_name(name)) {
      array_free(parts);
      array_free(sub_parts);
      continue;
    }

    char* value = array_get(sub_parts, 1);
    value = parse_cookie_value(value, true);
    array_free(sub_parts);

    if (!value) {
      array_free(parts);
      continue;
    }

    cookie_internal* c = (cookie_internal*)ys_cookie_init(name, value);

    for (unsigned int i = 1; i < array_size(parts); i++) {
      ((__array_t*)parts)->state[i] = s_trim(((__array_t*)parts)->state[i]);

      if (strlen(array_get(parts, i)) == 0) {
        continue;
      }

      array_t* attrs = str_cut(array_get(parts, i), "=");

      char* attr = array_get(attrs, 0);
      char* value = array_get(attrs, 1);
      array_free(attrs);

      value = parse_cookie_value(value, false);
      if (!value) {
        continue;
      }

      if (s_casecmp(attr, "samesite")) {
        if (!attr) {
          c->same_site = SAME_SITE_DEFAULT_MODE;
        } else if (s_casecmp(value, "lax")) {
          c->same_site = SAME_SITE_LAX_MODE;
        } else if (s_casecmp(value, "strict")) {
          c->same_site = SAME_SITE_STRICT_MODE;
        } else if (s_casecmp(value, "none")) {
          c->same_site = SAME_SITE_NONE_MODE;
        } else {
          c->same_site = SAME_SITE_DEFAULT_MODE;
        }
      } else if (s_casecmp(attr, "secure")) {
        c->secure = true;
      } else if (s_casecmp(attr, "httponly")) {
        c->http_only = true;
      } else if (s_casecmp(attr, "domain")) {
        c->domain = value;
      } else if (s_casecmp(attr, "max-age")) {
        int seconds = atoi(value);

        if (!seconds || (seconds != 0 && value[0] == '0')) {
          printlogf(YS_LOG_DEBUG, "max_age value %s is invalid", seconds);
        } else {
          if (seconds <= 0) {
            seconds = -1;
          }

          c->max_age = seconds;
        }
      } else if (s_casecmp(attr, "expires")) {
        struct tm tm_info = {0};

        // TODO: replace strptime
        if (strptime(value, COOKIE_TIME_FMT, &tm_info) != NULL) {
          time_t expires = timegm(&tm_info);
          c->expires = expires;
        }
      } else if (s_casecmp(attr, "path")) {
        c->path = value;
      }
    }

    array_free(parts);

    array_push(ret, c);
  }

  return ret;
}

ys_cookie* ys_cookie_init(const char* name, const char* value) {
  cookie_internal* c = xmalloc(sizeof(cookie_internal));

  c->domain = NULL;
  c->expires = -1;
  c->http_only = false;
  c->max_age = 0;  // 0 for this library means omit entirely
  c->name = s_copy(name);
  c->path = NULL;
  c->same_site = SAME_SITE_DEFAULT_MODE;
  c->secure = false;
  c->value = s_copy(value);

  return (ys_cookie*)c;
}

void ys_cookie_set_domain(ys_cookie* c, const char* domain) {
  ((cookie_internal*)c)->domain = s_copy(domain);
}

void ys_cookie_set_expires(ys_cookie* c, time_t when) {
  ((cookie_internal*)c)->expires = when;
}

void ys_cookie_set_http_only(ys_cookie* c, bool http_only) {
  ((cookie_internal*)c)->http_only = http_only;
}

void ys_cookie_set_max_age(ys_cookie* c, int age) {
  ((cookie_internal*)c)->max_age = age;
}

void ys_cookie_set_path(ys_cookie* c, const char* path) {
  ((cookie_internal*)c)->path = s_copy(path);
}

void ys_cookie_set_same_site(ys_cookie* c, ys_same_site_mode mode) {
  ((cookie_internal*)c)->same_site = mode;
}

void ys_cookie_set_secure(ys_cookie* c, bool secure) {
  ((cookie_internal*)c)->secure = secure;
}

char* ys_cookie_get_name(ys_cookie* c) { return ((cookie_internal*)c)->name; }

char* ys_cookie_get_value(ys_cookie* c) { return ((cookie_internal*)c)->value; }

char* ys_cookie_get_domain(ys_cookie* c) {
  return ((cookie_internal*)c)->domain;
}

time_t ys_cookie_get_expires(ys_cookie* c) {
  return ((cookie_internal*)c)->expires;
}

bool ys_cookie_get_http_only(ys_cookie* c) {
  return ((cookie_internal*)c)->http_only;
}

int ys_cookie_get_max_age(ys_cookie* c) {
  return ((cookie_internal*)c)->max_age;
}

char* ys_cookie_get_path(ys_cookie* c) { return ((cookie_internal*)c)->path; }

ys_same_site_mode ys_cookie_get_same_site(ys_cookie* c) {
  return ((cookie_internal*)c)->same_site;
}

bool ys_cookie_get_secure(ys_cookie* c) {
  return ((cookie_internal*)c)->secure;
}

ys_cookie* ys_get_cookie(ys_request* req, const char* name) {
  array_t* cookies = read_cookies(((request_internal*)req)->headers);

  foreach (cookies, i) {
    cookie_internal* c = (cookie_internal*)array_get(cookies, i);

    if (s_equals(c->name, name)) {
      return (ys_cookie*)c;
    }
  }

  return NULL;
}

void ys_set_cookie(ys_response* res, ys_cookie* c) {
  ys_set_header(res, SET_COOKIE, cookie_serialize((cookie_internal*)c));
}

void ys_cookie_free(ys_cookie* c) {
  cookie_internal* ci = (cookie_internal*)c;

  free(ci->name);
  free(ci->value);
  free(ci);
}
