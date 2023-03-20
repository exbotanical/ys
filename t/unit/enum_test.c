#include "libhttp.h"
#include "tap.c/tap.h"

void sanity_test_enum_macros() {
  is(http_method_names[METHOD_GET], "GET", "transforms method enum to name");
  is(http_status_names[STATUS_ACCEPTED], "ACCEPTED",
     "transforms status enum to name");
}

int main(int argc, char const *argv[]) {
  plan(2);
  sanity_test_enum_macros();
  done_testing();
  return 0;
}
