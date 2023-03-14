#include "util.h"

#include "libthread.h"

/**
 * @brief Designate the thread for suspension at the next suspension point
 *
 * @param thread
 */
void thread_suspend(thread_t* thread) {
	pthread_mutex_lock(&thread->mutex);

	if (IS_BIT_SET(thread->flags, THREAD_F_RUNNING)) {
		SET_BIT(thread->flags, THREAD_F_SUSPEND_PENDING);
	}

	pthread_mutex_unlock(&thread->mutex);
}

/**
 * @brief Create a suspension point
 *
 * @param thread
 */
void thread_test_suspend(thread_t* thread) {
	pthread_mutex_lock(&thread->mutex);

	if (IS_BIT_SET(thread->flags, THREAD_F_SUSPEND_PENDING)) {
		SET_BIT(thread->flags, THREAD_F_SUSPENDED);
		UNSET_BIT(thread->flags, THREAD_F_SUSPEND_PENDING);
		UNSET_BIT(thread->flags, THREAD_F_RUNNING);

		pthread_cond_wait(&thread->cv, &thread->mutex);
		// after signal, wake up and invoke routine
		SET_BIT(thread->flags, THREAD_F_RUNNING);
		(thread->thread_resume_routine)(thread->resume_arg);
	}

	pthread_mutex_unlock(&thread->mutex);
}

/**
 * @brief Resume a suspended thread
 *
 * @param thread
 */
void thread_resume(thread_t* thread) {
	pthread_mutex_lock(&thread->mutex);

  if (IS_BIT_SET(thread->flags, THREAD_F_SUSPENDED)) {
	  pthread_cond_signal(&thread->cv);
  }

	pthread_mutex_unlock(&thread->mutex);
}

/**
 * @brief Set callback invoked by a suspended thread immediately upon resuming;
 * it will be invoked before the thread continues the routine it was
 * executing prior to suspension
 *
 * @param thread
 * @param thread_routine
 * @param suspend_arg
 */
void thread_set_suspend_routine(
	thread_t* thread,
	void*(*thread_routine)(void*),
	void* suspend_arg
) {
	thread->thread_resume_routine = thread_routine;
	thread->resume_arg = suspend_arg;
}
