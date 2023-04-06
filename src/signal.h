#ifndef SIGNAL_H
#define SIGNAL_H

/**
 * Sets a SIGINT handler
 */
void setup_sigint_handler();

/**
 * Sets a SIGSEGV handler
 */
void setup_sigsegv_handler();

#endif /* SIGNAL_H */
