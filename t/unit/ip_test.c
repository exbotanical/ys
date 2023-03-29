#include "ip.h"

#include <stdbool.h>
#include <stdlib.h>

#include "tap.c/tap.h"

typedef struct {
  char* in;
  bool ok;
} test_case;

void test_validate_ip() {
  test_case tests[] = {{"127.0.1.2", true},
                       {"127.0.0.1", true},
                       {"::ffff:127.1.2.3", true},
                       {"::ffff:7f01:0203", true},
                       {"0:0:0:0:0000:ffff:127.1.2.3", true},
                       {"0:0:0:0:000000:ffff:127.1.2.3", true},
                       {"0:0:0:0::ffff:127.1.2.3", true},

                       {"2001:4860:0:2001::68", true},
                       {"2001:4860:0000:2001:0000:0000:0000:0068", true},

                       {"-0.0.0.0", false},
                       {"0.-1.0.0", false},
                       {"0.0.-2.0", false},
                       {"0.0.0.-3", false},
                       {"127.0.0.256", false},
                       {"abc", false},
                       {"123:", false},
                       {"fe80::1%lo0", false},
                       {"fe80::1%911", false},
                       {"", false},
                       {"a1:a2:a3:a4::b1:b2:b3:b4", false},
                       {"127.001.002.003", false},
                       {"::ffff:127.001.002.003", false},
                       {"123.000.000.000", false},
                       {"1.2..4", false},
                       {"0123.0.0.1", false}};

  for (unsigned int i = 0; i < sizeof(tests) / sizeof(test_case); i++) {
    test_case test = tests[i];

    ok(test.ok == validate_ip(test.in), "IP %s is %s", test.in,
       test.ok ? "valid" : "not valid");
  }
}

int main(int argc, char const* argv[]) {
  plan(25);

  test_validate_ip();

  done_testing();
}
