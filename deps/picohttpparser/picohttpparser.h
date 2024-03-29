/*
 * Copyright (c) 2009-2014 Kazuho Oku, Tokuhiro Matsuno, Daisuke Murase,
 *                         Shigeo Mitsunari
 *
 * The software is licensed under either the MIT License (below) or the Perl
 * license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

// Modified by Matthew Zito for libhttp
#ifndef picohttpparser_h
#define picohttpparser_h

#include <sys/types.h>

#ifdef _MSC_VER
#define ssize_t intptr_t
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* contains name and value of a header (name == NULL if is a continuing line
 * of a multiline header */
struct phr_header {
  char *name;
  size_t name_len;
  char *value;
  size_t value_len;
};

/* returns number of bytes consumed if successful, -2 if request is partial,
 * -1 if failed */
int phr_parse_request(char *buf, size_t len, char **method, size_t *method_len,
                      char **path, size_t *path_len, int *minor_version,
                      struct phr_header *headers, size_t *num_headers,
                      size_t last_len);

#ifdef __cplusplus
}
#endif

#endif
