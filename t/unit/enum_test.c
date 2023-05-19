#include "libys.h"
#include "tap.c/tap.h"
#include "tests.h"

void sanity_test_enum_macros(void) {
  is(ys_http_method_names[YS_METHOD_GET], "GET",
     "transforms method enum to name");
  is(ys_http_status_names[YS_STATUS_ACCEPTED], "Accepted",
     "gets the expected status string using the enum");
}

void run_enum_tests(void) { sanity_test_enum_macros(); }
