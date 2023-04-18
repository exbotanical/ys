#ifndef TESTS_H
#define TESTS_H

#include "libhash/libhash.h"
#include "libutil/libutil.h"

typedef struct {
  char* key;
  array_t* values;
} header;

header* to_header(char* key, array_t* values);

hash_table* to_headers(header* h, ...);

void run_cache_tests(void);
void run_config_tests(void);
void run_cookie_tests(void);
void run_cors_tests(void);
void run_enum_tests(void);
void run_header_tests(void);
void run_ip_tests(void);
void run_middleware_tests(void);
void run_path_tests(void);
void run_request_tests(void);
void run_response_tests(void);
void run_trie_tests(void);
void run_url_tests(void);
void run_util_tests(void);

#endif /* TESTS_H */
