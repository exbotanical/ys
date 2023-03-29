#ifndef COOKIE_H
#define COOKIE_H

#include <time.h>

#include "libhash/libhash.h"
#include "libhttp.h"
#include "libutil/libutil.h"

#define COOKIE_TIME_LEN 30

const char COOKIE_TIME_FMT[] = "%a, %d %b %Y %H:%M:%S GMT";

const char SET_COOKIE[] = "Set-Cookie";
const char COOKIE[] = "Cookie";

typedef enum {
  SAME_SITE_DEFAULT_MODE,
  SAME_SITE_NONE_MODE,
  SAME_SITE_LAX_MODE,
  SAME_SITE_STRICT_MODE,
} same_site_mode;

typedef struct {
  // IETF RFC 6265, Section 5.2.2
  // If delta-seconds is less than or equal to zero (0), let expiry-time
  // be the earliest representable date and time.  Otherwise, let the
  // expiry-time be the current date and time plus delta-seconds seconds.
  int max_age;
  time_t expires;
  same_site_mode same_site;
  char* name;
  char* value;
  char* path;
  char* domain;
  bool http_only;
  bool secure;
} cookie;

/**
 * get_cookie returns the named cookie provided in the request. If multiple
 * cookies match the given name, only the first matched cookie will be returned.
 *
 * @param req
 * @param name
 * @return cookie*
 */
cookie* get_cookie(request* req, const char* name);

/**
 * set_cookie adds a Set-Cookie header to the provided response's headers.
 * The provided cookie must have a valid Name. Invalid cookies may be silently
 * omitted.
 * @param res
 * @param c
 */
void set_cookie(response* res, cookie* c);

/**
 * read_cookies parses all "Cookie" values from the given headers and returns
 * the successfully parsed cookies
 *
 * @param headers
 * @return array_t*
 */
array_t* read_cookies(hash_table* headers);

/**
 * Initialize a new Cookie with a given name and value. All other fields will be
 * initialized to NULL or field-equivalent values such that the field is
 * ignored. Use the cookie_set_* helpers to set the other attributes.
 *
 * @param name
 * @param value
 * @return cookie*
 */
cookie* cookie_init(const char* name, const char* value);

/**
 * Set the given Cookie's domain attribute
 *
 * @param c
 * @param domain
 */
void cookie_set_domain(cookie* c, const char* domain);

/**
 * Set the given Cookie's expires attribute
 *
 * @param c
 * @param when
 */
void cookie_set_expires(cookie* c, time_t when);

/**
 * Set the given Cookie's http_only attribute
 *
 * @param c
 * @param http_only
 */
void cookie_set_http_only(cookie* c, bool http_only);

/**
 * Set the given Cookie's age attribute
 *
 * @param c
 * @param age
 */
void cookie_set_max_age(cookie* c, int age);

/**
 * Set the given Cookie's path attribute
 *
 * @param c
 * @param path
 */
void cookie_set_path(cookie* c, const char* path);

/**
 * Set the given Cookie's same_site attribute
 *
 * @param c
 * @param mode
 */
void cookie_set_same_site(cookie* c, same_site_mode mode);

/**
 * Set the given Cookie's secure attribute
 *
 * @param c
 * @param secure
 */
void cookie_set_secure(cookie* c, bool secure);

#endif /* COOKIE_H */
