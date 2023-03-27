#include "libhttp.h"
#include "tap.c/tap.h"

void sanity_test_enum_macros() {
  is(http_method_names[METHOD_GET], "GET", "transforms method enum to name");
  is(http_status_names[STATUS_ACCEPTED], "Accepted",
     "gets the expected status string using the enum");
}

int main() {
  plan(2);

  sanity_test_enum_macros();

  done_testing();
}
