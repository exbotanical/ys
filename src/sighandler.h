#ifndef SIGNAL_H
#define SIGNAL_H

/**
 * Sets a SIGINT handler
 */
void setup_sigint_handler(void);

/**
 * Sets a SIGSEGV handler
 */
void setup_sigsegv_handler(void);

#endif /* SIGNAL_H */
