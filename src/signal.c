#include "signal.h"

#include <signal.h>
#include <stdlib.h>  // for exit code macros

#include "logger.h"

void sigint_handler() { DIE("Caught SIGINT signal, shutting down...\n"); }
void setup_sigint_handler() { signal(SIGINT, sigint_handler); }

void segfault_handler() { DIE("Caught SIGSEGV signal, shutting down...\n"); }
void setup_sigsegv_handler() { signal(SIGSEGV, segfault_handler); }
