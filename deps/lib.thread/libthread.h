#ifndef LIB_THREAD_H
#define LIB_THREAD_H

#include "lib.cartilage/libcartilage.h" /* for glthread, an impl of doubly linked list */

#include <pthread.h> /* for POSIX threads and related functionality */
#include <stdbool.h> /* for common boolean typedefs */
#include <stdint.h> /* for extended type signatures */

/* Thread Flags */

/* Thread is running its routine as is typical */
#define THREAD_F_RUNNING (1 << 0)
/* Thread has been marked for suspension, but is not yet in a suspended state */
#define THREAD_F_SUSPEND_PENDING (1 << 1)
/* Thread is suspended */
#define THREAD_F_SUSPENDED (1 << 2)
/* Thread is blocked on its condition variable (and not suspended)*/
#define THREAD_F_BLOCKED (1 << 3)

/* Canonical Aliases */

#define P(sem) semaphore_wait(sem)
#define V(sem) semaphore_post(sem)

#define UP(sem) semaphore_wait(sem)
#define DOWN(sem) semaphore_post(sem)

/* Structures */

typedef struct semaphore {
  int permit_counter;
  pthread_cond_t cv;
  pthread_mutex_t mutex; /* Controls access to `permit_counter` */
} semaphore_t;

typedef struct barrier {
	/* The maximum thread threshold until the barrier blocks */
  uint32_t threshold;
	/* A wait period - from 0 -> threshold - 1*/
  uint32_t curr_wait;
  pthread_cond_t cv;
	/* Mutex for performing ops on a thread barrier */
  pthread_mutex_t mutex;
	/* State flag: is barrier disposition in progress? */
  bool is_ready;
	 /* Condition Variable: if thread barrier in 'disposition progress' state, block inbound threads */
  pthread_cond_t busy_cv;
} barrier_t;

/**
 * @brief Describes the execution unit
 * data structure of a thread
 */
typedef struct thread {
	/* Name identifier for the thread */
  char name[32];
	/* Has the thread member been initialized? */
  bool thread_created;
  /* Actual pthread_t */
	pthread_t thread;
  /* Data passed to the thread routine */
	void* arg;
  /* The routine executed upon thread creation */
	void* (*thread_routine)(void*);
	/* Data passed to the thread resume routine */
	void* resume_arg;
	/* The routine executed when the thread awakens from suspension */
  void* (*thread_resume_routine)(void*);
	/* The pthread_t member's attributes */
	pthread_attr_t attrs;
	/* The pthread_t member's semaphore structure */
  semaphore_t* semaphore;
	/* The pthread_t member's condition variable */
  pthread_cond_t cv;
	/* Update thread state in a mutually exclusive manner */
	pthread_mutex_t mutex;
	/* Bitflags for tracking thread state */
	uint32_t flags;
	/* A pointer to the linked list node to which the thread belongs - used by thread pools */
	glthread_t glthread;
} thread_t;

/**
 * @brief A thread pool, implemented as a
 * doubly linked list with offset data storage
 */
typedef struct thread_pool {
	/* Head node of thread pool */
	glthread_t head;
	pthread_mutex_t mutex;
} thread_pool_t;

/**
 * @brief Represents thread execution data for
 * different states
 */
typedef struct thread_exec_data {
	/* Routine invoked at the thread execution stage */
	void* (*exec_routine)(void*);
	void* exec_arg;
	/*Routine invoked at the thread resuspend stage 22*/
	void* (*resuspend_routine)(thread_pool_t*, thread_t*);
	thread_pool_t* pool;
	thread_t* thread;
} thread_exec_data_t;

/* Weak Semaphore API */

void semaphore_init(semaphore_t* sem, int count);

void semaphore_wait(semaphore_t* sem);

void semaphore_post(semaphore_t* sem);

void semaphore_destroy(semaphore_t* sem);

int semaphore_reveal(semaphore_t* sem);

/* Thread Barrier API */
void barrier_init(barrier_t* barrier, uint32_t threshold);

void barrier_wait(barrier_t* barrier);

/* Stateful POSIX Thread API */

thread_t* thread_init(thread_t* thread, char* name);

bool thread_run(thread_t* thread, void*(*thread_routine)(void*), void* arg);

void thread_set_attr(thread_t *thread, bool joinable);

/* Suspensions API */

void thread_suspend(thread_t* thread);

void thread_test_suspend(thread_t* thread);

void thread_resume(thread_t* thread);

void thread_set_suspend_routine(
	thread_t* thread,
	void*(*thread_routine)(void*),
	void* suspend_arg
);

/* Thread Pool API */

void thread_pool_init(thread_pool_t* pool);

void thread_pool_insert(thread_pool_t* pool, thread_t* thread);

thread_t* thread_pool_get(thread_pool_t* pool);

bool thread_pool_dispatch(
	thread_pool_t* pool,
	void*(*thread_routine)(void*),
	void* routine_arg,
	bool block_caller
);

#endif /* LIB_THREAD_H */
