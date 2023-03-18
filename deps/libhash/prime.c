#include "prime.h"

#include <math.h>

/**
 * Determine whether the given integer `x` is prime.
 *
 * @param x
 * @return int
 */
int is_prime(const int x) {
  if (x < 2) {
    return 0;
  }
  if (x < 4) {
    return 1;
  }
  if ((x % 2) == 0) {
    return 0;
  }
  for (int i = 3; i <= floor(sqrt((double)x)); i += 2) {
    if ((x % i) == 0) {
      return 0;
    }
  }
  return 1;
}

/**
 * Return the next prime after the given integer `x`,
 * or `x` if `x` is prime. Successive, brute-force resolution.
 *
 * @param x
 * @return int
 */
int next_prime(int x) {
  while (!is_prime(x)) {
    x++;
  }

  return x;
}
