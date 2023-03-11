#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void *thread_routine(void *arg) {
  int *iterations = malloc(sizeof(*iterations));
  *iterations = (rand() % 10) + 1;

  int iterations_mut = *iterations;
  while (iterations_mut--) {
    printf("current iteration: %d\n", iterations_mut + 1);

    sleep(1);
  }

  return iterations;  // or pthread_exit(iterations);
}

int main(int argc, char const *argv[]) {
  srand(time(NULL));  // Initialization; should only be called once

  pthread_t thread_id;

  // PTHREAD_CREATE_JOINABLE

  if (pthread_create(&thread_id, NULL, thread_routine, NULL) != 0) {
    exit(1);  // error
  }

  int *iterations;
  pthread_join(
      thread_id,
      (void *)&iterations);  // this blocks until `thread_id` finishes execution

  printf("thread ran %d iterations\n", *iterations);

  return 0;
}
