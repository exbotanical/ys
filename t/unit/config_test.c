#include "config.c"

#include "tap.c/tap.h"

void reset_server_conf() {
  server_conf.log_file = NULL;
  server_conf.log_level = DEFAULT_LOG_LEVEL;
  server_conf.threads = DEFAULT_NUM_THREADS;
  server_conf.port = DEFAULT_PORT_NUM;
}

void test_config_defaults() {
  is(server_conf.log_file, NULL, "log file is NULL by default");
  is(server_conf.log_level, DEFAULT_LOG_LEVEL, "default log level is set");
  ok(server_conf.threads == DEFAULT_NUM_THREADS,
     "default number of threads is set");
  ok(server_conf.port == DEFAULT_PORT_NUM, "default port number is set");
}

void test_parse_config_ok() {
  const char* test_config = "t/unit/fixtures/test.conf";

  ok(parse_config(test_config) == true, "parsing a valid config returns true");

  is(server_conf.log_file, "server.log",
     "log file is what's specified in config");
  is(server_conf.log_level, "debug", "log level is what's specified in config");
  ok(server_conf.threads == 4,
     "number of threads is what's specified in config");
  ok(server_conf.port == 8000, "port number is what's specified in config");
}

void test_parse_config_ok_empty() {
  const char* test_config = "t/unit/fixtures/invalid2.conf";

  ok(parse_config(test_config) == true, "parsing an empty config returns true");

  is(server_conf.log_file, NULL, "log file is default when config is empty");
  is(server_conf.log_level, DEFAULT_LOG_LEVEL,
     "log level is default when config is empty");
  ok(server_conf.threads == DEFAULT_NUM_THREADS,
     "number of threads is default when config is empty");
  ok(server_conf.port == DEFAULT_PORT_NUM,
     "port number is default when config is empty");
}

void test_parse_config_ok_no_config() {
  const char* test_config = "nope";

  ok(parse_config(test_config) == true,
     "parse_config with no config file returns true");

  is(server_conf.log_file, NULL, "log file is default when no config");
  is(server_conf.log_level, DEFAULT_LOG_LEVEL,
     "log level is default when no config");
  ok(server_conf.threads == DEFAULT_NUM_THREADS,
     "number of threads is default when no config");
  ok(server_conf.port == DEFAULT_PORT_NUM,
     "port number is default when no config");
}

void test_parse_config_err_missing_value() {
  const char* test_config = "t/unit/fixtures/invalid.conf";

  ok(parse_config(test_config) == false,
     "parsing a config with a missing value returns false");

  is(server_conf.log_file, NULL, "log file is default when parse_config fails");
  is(server_conf.log_level, DEFAULT_LOG_LEVEL,
     "log level is default when parse_config fails");
  ok(server_conf.threads == DEFAULT_NUM_THREADS,
     "number of threads is default when parse_config fails");
  ok(server_conf.port == DEFAULT_PORT_NUM,
     "port number is default when parse_config fails");
}

void test_parse_config_err_unknown_opt() {
  const char* test_config = "t/unit/fixtures/invalid3.conf";

  ok(parse_config(test_config) == false,
     "parsing a config with an unknown option returns false");

  is(server_conf.log_file, NULL, "log file is default when parse_config fails");
  is(server_conf.log_level, DEFAULT_LOG_LEVEL,
     "log level is default when parse_config fails");
  ok(server_conf.threads == DEFAULT_NUM_THREADS,
     "number of threads is default when parse_config fails");
  ok(server_conf.port == DEFAULT_PORT_NUM,
     "port number is default when parse_config fails");
}

int main() {
  plan(29);

  test_config_defaults();
  reset_server_conf();

  test_parse_config_ok();
  reset_server_conf();

  test_parse_config_err_missing_value();
  reset_server_conf();

  test_parse_config_ok_empty();
  reset_server_conf();

  test_parse_config_err_unknown_opt();
  reset_server_conf();

  test_parse_config_ok_no_config();
  reset_server_conf();

  done_testing();
}
