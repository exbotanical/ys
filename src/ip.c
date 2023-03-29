// Code ported from the Go source code. Here's their license:

// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.
#include "ip.h"

#include <stdlib.h>
#include <string.h>  // for strlen

#include "libutil/libutil.h"
#include "util.h"

// Bigger than we need, not too big to worry about overflow
const int BIG = 0xFFFFFF;

// IP address lengths (bytes)
const int IPv4len = 4;
const int IPv6len = 16;

const int v4InV6Prefix[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff};

/**
 * xtoi performs a hexadecimal to int conversion.
 *
 * @param s
 * @return int* An int array of [retval, n chars consumed, ok]
 */
static int* xtoi(char* s) {
  int* ret = malloc(3 * sizeof(int));

  int n = 0;
  int i = 0;

  for (; i < (int)strlen(s); i++) {
    if ('0' <= s[i] && s[i] <= '9') {
      n *= 16;
      n += s[i] - '0';
    } else if ('a' <= s[i] && s[i] <= 'f') {
      n *= 16;
      n += s[i] - 'a' + 10;
    } else if ('A' <= s[i] && s[i] <= 'F') {
      n *= 16;
      n += s[i] - 'A' + 10;
    } else {
      break;
    }

    if (n >= BIG) {
      ret[0] = 0;
      ret[1] = i;
      ret[2] = false;
      return ret;
    }
  }

  if (i == 0) {
    ret[0] = 0;
    ret[1] = i;
    ret[2] = false;
    return ret;
  }

  ret[0] = n;
  ret[1] = i;
  ret[2] = true;
  return ret;
}

/**
 * dtoi performs a decimal to integer conversion.
 *
 * @param s
 * @return int* An int array of [retval, n chars consumed, ok]
 */
static int* dtoi(char* s) {
  int* ret = malloc(3 * sizeof(int));

  int n = 0;
  int i = 0;

  for (; i < (int)strlen(s) && '0' <= s[i] && s[i] <= '9'; i++) {
    n = n * 10 + s[i] - '0';
    if (n >= BIG) {
      ret[0] = BIG;
      ret[1] = i;
      ret[2] = false;

      return ret;
    }
  }

  if (i == 0) {
    ret[0] = 0;
    ret[1] = 0;
    ret[2] = false;

    return ret;
  }

  ret[0] = n;
  ret[1] = i;
  ret[2] = true;

  return ret;
}

/**
 * to_ipv4 formats 4 given integers into a valid IPv4 address
 *
 * @param a
 * @param b
 * @param c
 * @param d
 * @return int* An int array representing the IPv4 address
 */
static int* to_ipv4(int a, int b, int c, int d) {
  int* p = malloc(IPv6len * sizeof(int));

  for (int i = 0; i < 12; i++) {
    p[i] = v4InV6Prefix[i];
  }

  p[12] = a;
  p[13] = b;
  p[14] = c;
  p[15] = d;

  return p;
}

/**
 * parse_ipv4 parses s into a int array representing an IPv4 address
 * (d.d.d.d)
 *
 * @param s
 * @return int*
 */
static int* parse_ipv4(const char* s) {
  char* cp = s_copy(s);

  char p[IPv4len];
  for (int i = 0; i < IPv4len; i++) {
    if (strlen(cp) == 0) {
      // Missing octets
      return NULL;
    }

    if (i > 0) {
      if (cp[0] != '.') {
        return NULL;
      }

      cp = s_substr(cp, 1, strlen(cp), true);
    }

    int* digits = dtoi(cp);

    int n = digits[0];
    int c = digits[1];
    int ok = digits[2];

    if (!ok || n > 0xFF) {
      return NULL;
    }

    if (c > 1 && cp[0] == '0') {
      // Reject non-zero components with leading zeroes
      return NULL;
    }

    cp = s_substr(cp, c, strlen(cp), true);

    p[i] = n;
  }

  if (strlen(cp) != 0) {
    return NULL;
  }

  return to_ipv4(p[0], p[1], p[2], p[3]);
}

/**
 * validate_ipv6 validates whether s is a valid IPv6 address as described in RFC
 * 4291 and RFC 5952
 *
 * @param s
 * @return true
 * @return false
 */
static bool validate_ipv6(const char* s) {
  char* cp = s_copy(s);
  int* ip = malloc(IPv6len * sizeof(int));

  int ellipsis = -1;  // Position of ellipsis in ip

  // Might have leading ellipsis
  if (strlen(cp) >= 2 && cp[0] == ':' && cp[1] == ':') {
    ellipsis = 0;
    cp = s_substr(cp, 2, strlen(cp), true);
    // Might be only ellipsis
    if (strlen(cp) == 0) {
      return true;
    }
  }

  // Loop, parsing hex numbers followed by colon
  int i = 0;
  while (i < IPv6len) {
    // Hex number
    int* digits = xtoi(cp);
    int n = digits[0];
    int c = digits[1];
    int ok = digits[2];

    if (!ok || n > 0xFFFF) {
      return false;
    }

    // If followed by dot, might be in trailing IPv4
    if (c < (int)strlen(cp) && cp[c] == '.') {
      if (ellipsis < 0 && i != IPv6len - IPv4len) {
        // Not the right place
        return false;
      }

      if (i + IPv4len > IPv6len) {
        // Not enough room
        return false;
      }

      int* ip4 = parse_ipv4(cp);
      if (!ip4) {
        return false;
      }

      ip[i] = ip4[12];
      ip[i + 1] = ip4[13];
      ip[i + 2] = ip4[14];
      ip[i + 3] = ip4[15];

      cp = "";
      i += IPv4len;
      break;
    }

    // Save this 16-bit chunk
    ip[i] = n >> 8;
    ip[i + 1] = n;
    i += 2;

    // Stop at end of string
    cp = s_substr(cp, c, strlen(cp), true);
    if (strlen(cp) == 0) {
      break;
    }

    // Otherwise must be followed by colon and more
    if (cp[0] != ':' || strlen(cp) == 1) {
      return false;
    }

    cp = s_substr(cp, 1, strlen(cp), true);
    // TODO: free cp
    //  Look for ellipsis
    if (cp[0] == ':') {
      if (ellipsis >= 0) {
        // Already have one
        return false;
      }

      ellipsis = i;
      cp = s_substr(cp, 1, strlen(cp), true);
      if (strlen(cp) == 0) {
        // Can be at end
        break;
      }
    }
  }

  // Must have used entire string
  if (strlen(cp) != 0) {
    return false;
  }

  // If didn't parse enough, expand ellipsis
  if (i < IPv6len) {
    if (ellipsis < 0) {
      return false;
    }

    int n = IPv6len - i;
    for (int j = i - 1; j >= ellipsis; j--) {
      ip[j + n] = ip[j];
    }

    for (int j = ellipsis + n - 1; j >= ellipsis; j--) {
      ip[j] = 0;
    }
  } else if (ellipsis >= 0) {
    // Ellipsis must represent at least one 0 group
    return false;
  }

  return true;
}

bool validate_ip(const char* s) {
  for (int i = 0; i < (int)strlen(s); i++) {
    switch (s[i]) {
      case '.':
        return !!parse_ipv4(s);
      case ':':
        return validate_ipv6(s);
    }
  }

  return false;
}
