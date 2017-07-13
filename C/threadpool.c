#include <stdlib.h>
#include "threadpool.h"

#define TRUE 1

/* New Thread Pool */
PoolThread *createPool(unsigned int n_threads) 
{
	PoolThread *pt = (PoolThread*) malloc(sizeof(PoolThread));
	pt->threads = (pthread_t*) malloc(sizeof(pthread_t)*n_threads);
	pt->threads_queue = newQueue();
	pt->pool_size = n_threads;
	pt->n_working = 0;
	pt->queue_mutex = (sem_t*) malloc(sizeof(sem_t)); // Queue's mutex
	pt->job_sem = (sem_t*) malloc(sizeof(sem_t)); // Job's control semaphore
	pt->w_mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)); // Working's counter mutex
	pt->idle = (pthread_cond_t*) malloc(sizeof(pthread_cond_t)); // Control working pool

	sem_init(pt->queue_mutex,0,1);
	sem_init(pt->job_sem,0,0);
	pthread_mutex_init(pt->w_mutex, NULL);
	pthread_cond_init(pt->idle, NULL);

	/* Thread's creation */
	for(int i=0; i < n_threads; i++) 
		pthread_create(&(pt->threads[i]), NULL, _worker, pt);

	return pt;
}

void deletePool(PoolThread *pt)
{
	Job *t, *tt;
	for(t = pt->threads_queue->first; t!= NULL; t=tt)
	{
		tt = t->next;
		free(t);
	}
	
	free(pt->threads_queue);
	free(pt->threads);
	free(pt);
}

void *_worker(void *vp)
{
	int tmp;
	PoolThread *pt = (PoolThread*) vp;
  	Job *job;
	while (TRUE) {
		sem_wait(pt->queue_mutex);
		if (pt->threads_queue->size != 0) 
		{
			pthread_mutex_lock(pt->w_mutex);
			pt->n_working++;
		  	pthread_mutex_unlock(pt->w_mutex);
		  	job = (Job*) extractToQueue(pt->threads_queue);
		  	sem_post(pt->queue_mutex);
		  	job->func(job->args);
		  	free(job);
		  	pthread_mutex_lock(pt->w_mutex);
		  	pt->n_working--;
		  	if (!pt->n_working)
		    	pthread_cond_signal(pt->idle);
	  		pthread_mutex_unlock(pt->w_mutex);
		} 
		else 
		{
	  		sem_post(pt->queue_mutex);
	  		sem_wait(pt->job_sem);
		}
	}
}

void poolSendJob(PoolThread *pt, void *func, void *args)
{
	Job *job = (Job*) malloc(sizeof(Job));
	job->func = func;
	job->args = args;
	job->next = NULL;
	sem_wait(pt->queue_mutex);
	addToQueue(pt->threads_queue, job);
	sem_post(pt->queue_mutex);
	sem_post(pt->job_sem);
}

void poolWait(PoolThread *pt) 
{
	pthread_mutex_lock(pt->w_mutex);
	while (pt->n_working || pt->threads_queue->size) {
		pthread_cond_wait(pt->idle, pt->w_mutex);
  	}
  	pthread_mutex_unlock(pt->w_mutex);
}
