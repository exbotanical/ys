#include <time.h>

#define ONE_MINUTE_IN_SECONDS 60

time_t n_minutes_from_now(unsigned int n) {
  return time(NULL) + (ONE_MINUTE_IN_SECONDS * n);
}

time_t n_hours_from_now(unsigned int n) {
  return time(NULL) + (ONE_MINUTE_IN_SECONDS * 60 * n);
}

time_t n_days_from_now(unsigned int n) {
  return time(NULL) + (ONE_MINUTE_IN_SECONDS * 60 * 24 * n);
}
