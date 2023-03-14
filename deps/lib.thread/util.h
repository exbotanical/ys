#ifndef LIBTHREAD_UTIL_H
#define LIBTHREAD_UTIL_H

#define INFINITY 0xFFFFFFFF
#define IS_BIT_SET(n, bit) (n & bit)
#define TOGGLE_BIT(n, bit) (n = (n ^ (bit)))
#define COMPLEMENT(n) (n = (n ^ INFINITY))
#define SET_BIT(n, bit) (n = (n | (bit)))
#define UNSET_BIT(n, bit) (n = (n & ((bit) ^ INFINITY)))

#endif /* LIBTHREAD_UTIL_H */
