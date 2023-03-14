#include "libthread.h"

/**
 * @brief Initialize a semaphore of `count`
 *
 * @param sem
 * @param count
 * @return void
 */
void semaphore_init(semaphore_t* sem, int count) {
  sem->permit_counter = count;
  pthread_cond_init(&sem->cv, NULL);
  pthread_mutex_init(&sem->mutex, NULL);
}

/**
 * @brief Lock the semaphore
 *
 * @param sem
 * @return void
 */
void semaphore_wait(semaphore_t* sem) {
  pthread_mutex_lock(&sem->mutex);
  sem->permit_counter--;

  // a thread was blocked
  if (sem->permit_counter < 0) {
    pthread_cond_wait(&sem->cv, &sem->mutex);
  }

  pthread_mutex_unlock(&sem->mutex);
}

/**
 * @brief Signal to the semaphore that a single thread's queue lifecycle has completed
 *
 * @param sem
 * @return void
 */
void semaphore_post(semaphore_t* sem) {
  bool has_waiting_thread;
  pthread_mutex_lock(&sem->mutex);

  has_waiting_thread = sem->permit_counter < 0 ? true : false;
  sem->permit_counter++;

  if (has_waiting_thread) {
    pthread_cond_signal(&sem->cv);
  }

  pthread_mutex_unlock(&sem->mutex);
}

/**
 * @brief Destroy the given semaphore
 *
 * @param sem
 * @return void
 */
void semaphore_destroy(semaphore_t* sem) {
  sem->permit_counter = 0;
  pthread_mutex_unlock(&sem->mutex);
  pthread_cond_destroy(&sem->cv);
  pthread_mutex_destroy(&sem->mutex);
}

/**
 * @brief Access the `permit_counter` data member of the given semaphore
 *
 * @param sem
 * @return int
 */
int semaphore_reveal(semaphore_t* sem) {
  return sem->permit_counter;
}
