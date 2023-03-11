#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void* detached_thread_routine(void* arg) {
  char* thread_id = (char*)arg;
  while (1) {
    printf("%s\n", thread_id);

    sleep(1);
  }

  pthread_exit(NULL);
}

void* create_thread_routine(void* arg) {
  pthread_t detached_thread_id;
  pthread_attr_t attr;

  pthread_attr_setdetachstate(
      &attr, PTHREAD_CREATE_DETACHED);  // set the thread mode to detached

  static char* thread_arg = "detached thread";
  if (pthread_create(&detached_thread_id, &attr, detached_thread_routine,
                     (void*)thread_arg) != 0) {
    exit(1);  // error
  }
}

int main(int argc, char const* argv[]) {
  pthread_t joinable_thread_id;

  if (pthread_create(&joinable_thread_id, NULL, create_thread_routine, NULL) !=
      0) {
    exit(1);  // error
  }

  printf("blocked\n");
  pthread_join(joinable_thread_id, NULL);
  printf("joined\n");

  pause();

  return 0;
}
