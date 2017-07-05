#ifndef POOLTHREAD_H_
#define POOLTHREAD_H_

#include <pthread.h>
#include <semaphore.h>
#include "queue.h"


struct _pool_thread
{
	pthread_t *threads;
	queue *queue_threads;
	sem_t *queue_mutex, *st;
	pthread_mutex_t *cmutex;
	pthread_cond_t *idle;
	unsigned int pool_size, n_working;
} typedef poolThread;


poolThread *createPool(unsigned int n_threads);
void deletePool(poolThread *pt);
void *_worker(void *vp); 
void poolSendJob(poolThread *pt, void *func, void *args);
void poolWait(poolThread *pt);

#endif