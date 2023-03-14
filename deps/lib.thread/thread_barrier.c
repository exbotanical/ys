/**
 * @file thread_barrier.c
 * @author Matthew Zito (goldmund@freenode)
 * @brief A barrier that activates after `threshold` threads have at
 * any given time attempted to access the surrounded Critical Section
 *
 * @version 0.1
 * @date 2021-07-18
 *
 * @copyright Copyright (c) 2021 Matthew Zito
 *
 */

#include "libthread.h"

/**
 * @brief Initialize the barrier and its condition variable, mutex
 *
 * @param barrier
 * @param threshold
 */
void barrier_init(barrier_t* barrier, uint32_t threshold) {
  barrier->threshold = threshold;
  barrier->curr_wait = 0;

  pthread_cond_init(&barrier->cv, NULL);
  pthread_mutex_init(&barrier->mutex, NULL);

  barrier->is_ready = true;
  pthread_cond_init(&barrier->busy_cv, NULL);
}

/**
 * @brief Enforce the barrier's waiting period, of `threshold`
 *
 * @param barrier
 */
void barrier_wait(barrier_t* barrier) {
  pthread_mutex_lock(&barrier->mutex);

  while (!barrier->is_ready) {
    pthread_cond_wait(&barrier->busy_cv, &barrier->mutex);
  }

  if (barrier->curr_wait + 1 == barrier->threshold) {
    barrier->is_ready = false;

    pthread_cond_signal(&barrier->cv);
    pthread_mutex_lock(&barrier->mutex);

    return;
  }

  barrier->curr_wait++;
  pthread_cond_wait(&barrier->cv, &barrier->mutex);
  barrier->curr_wait--;

  if (barrier->curr_wait == 0) {
    barrier->is_ready = true;
    pthread_cond_broadcast(&barrier->cv);
  } else {
    pthread_cond_signal(&barrier->cv);
  }

  pthread_mutex_lock(&barrier->mutex);
}
