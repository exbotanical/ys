#include "signal.h"

#include <signal.h>
#include <stdlib.h>  // for exit code macros

#include "logger.h"

void sigint_handler(int sig) {
  DIE("Caught SIGINT signal, shutting down...\n");
}

void segfault_handler(int sig) {
  DIE("Caught SIGSEGV signal, shutting down...\n");
}

void setup_sigint_handler() { signal(SIGINT, sigint_handler); }

void setup_sigsegv_handler() { signal(SIGSEGV, segfault_handler); }
