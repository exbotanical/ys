#include "sighandler.h"

#include <signal.h>
#include <stdlib.h>

#include "logger.h"

static void sigint_handler(void) {
  DIE("Caught SIGINT signal, shutting down...\n");
}
void setup_sigint_handler(void) { signal(SIGINT, sigint_handler); }

static void segfault_handler(void) {
  DIE("Caught SIGSEGV signal, shutting down...\n");
}
void setup_sigsegv_handler(void) { signal(SIGSEGV, segfault_handler); }
