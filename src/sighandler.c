#include "sighandler.h"

#include <signal.h>
#include <stdlib.h>

#include "logger.h"

static void sigint_handler(int s) {
  DIE("Caught SIGINT (%d) signal, shutting down...\n", s);
}
void setup_sigint_handler(void) { signal(SIGINT, sigint_handler); }

static void segfault_handler(int s) {
  DIE("Caught SIGSEGV (%d) signal, shutting down...\n", s);
}
void setup_sigsegv_handler(void) { signal(SIGSEGV, segfault_handler); }
