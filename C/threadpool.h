#ifndef POOLTHREAD_H_
#define POOLTHREAD_H_

#include <pthread.h>
#include <semaphore.h>

struct _job
{
	void (*func)(void* args);
	void *args;
	job *next;
} typedef job;

struct _queue
{ 
  job *first;
  job *last;
  //unsigned int size;
} typedef queue;

struct _pool_thread
{
	pthread_t *threads;
	queue *queue_threads;
	unsigned int pool_size, n_working;
} typedef poolThread;


poolThread *createPool(unsigned int n_threads);
void deletePool(poolThread *pt);
//void *_worker(void *vp); 
void poolSendJob(poolThread *pt, void *func, void *args);
void poolWait(poolThread *pt);

#endif