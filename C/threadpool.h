#ifndef POOLTHREAD_H_
#define POOLTHREAD_H_

#include <pthread.h>
#include <semaphore.h>
#include "queue.h"


struct _pool_thread
{
	pthread_t *threads;
	Queue *threads_queue;
	sem_t *queue_mutex, *job_sem;
	pthread_mutex_t *w_mutex;
	pthread_cond_t *idle;
	unsigned int pool_size, n_working;
} typedef PoolThread;


PoolThread *createPool(unsigned int n_threads);
void deletePool(PoolThread *pt);
void *_worker(void *vp); 
void poolSendJob(PoolThread *pt, void *func, void *args);
void poolWait(PoolThread *pt);

#endif