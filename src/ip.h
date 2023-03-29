#ifndef IP_H
#define IP_H

#include <stdbool.h>

/**
 * validate_ip parses s as an IP address, returning a boolean indicating whether
 * it is valid.
 *
 * The string s can be in IPv4 dotted decimal ("192.0.2.1"), IPv6
 * ("2001:db8::68"), or IPv4-mapped IPv6 ("::ffff:192.0.2.1") form. If s is not
 * a valid textual representation of an IP address, validate_ip returns false.
 *
 * @param s
 * @return true
 * @return false
 */
bool validate_ip(const char* s);

#endif /* IP_H */
