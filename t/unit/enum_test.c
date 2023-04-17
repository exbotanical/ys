#include "libys.h"
#include "tap.c/tap.h"
#include "tests.h"

void sanity_test_enum_macros(void) {
  is(http_method_names[METHOD_GET], "GET", "transforms method enum to name");
  is(http_status_names[STATUS_ACCEPTED], "Accepted",
     "gets the expected status string using the enum");
}

void run_enum_tests(void) { sanity_test_enum_macros(); }
