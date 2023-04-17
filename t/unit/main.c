#include "tap.c/tap.h"
#include "tests.h"

int main() {
  plan(558);

  run_cache_tests();
  run_config_tests();
  run_cookie_tests();
  run_cors_tests();
  run_enum_tests();
  run_header_tests();
  run_ip_tests();
  run_middleware_tests();
  run_path_tests();
  run_request_tests();
  run_response_tests();
  run_trie_tests();
  run_url_tests();
  run_util_tests();

  done_testing();
}
