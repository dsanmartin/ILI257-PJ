#include <stdlib.h>
#include "threadpool.h"

#define TRUE 1

poolThread *createPool(unsigned int n_threads) 
{
	poolThread *pt = (poolThread*) malloc(sizeof(poolThread));
	pt->threads = (pthread_t*) malloc(sizeof(pthread_t)*n_threads);
	
	/*
	pt->queue_threads = (queue*) malloc(sizeof(queue));
	pt->queue_threads->first = NULL;
	pt->queue_threads->last = NULL;
	*/

	pt->queue_threads = newQueue();
	pt->pool_size = n_threads;
	pt->n_working = 0;

	pt->queue_mutex = (sem_t*) malloc(sizeof(sem_t));
	pt->st = (sem_t*) malloc(sizeof(sem_t));
	pt->cmutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t)); // Working's counter mutex
	pt->idle = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));

	sem_init(pt->queue_mutex,0,1);
	sem_init(pt->st,0,0);
	pthread_mutex_init(pt->cmutex, NULL);
	pthread_cond_init(pt->idle, NULL);

	for(int i=0; i < n_threads; i++) 
		pthread_create(&(pt->threads[i]), NULL, _worker, pt);

	return pt;
}

void deletePool(poolThread *pt)
{
	// Deberia borrar los threads tambien
	job *t, *tt;
	for(t = pt->queue_threads->first; t!= NULL; t=tt)
	{
		tt = t->next;
		free(t);
	}
	
	free(pt->queue_threads);
	free(pt->threads);
	free(pt);
}

void *_worker(void *vp)
{
	int tmp;
	poolThread *pt = (poolThread*) vp;
  	job *my_job;
	while (TRUE) {
	sem_wait(pt->queue_mutex);
	if (pt->queue_threads->size != 0) 
	{
		pthread_mutex_lock(pt->cmutex);
		pt->n_working++;
	  	pthread_mutex_unlock(pt->cmutex);
	  	my_job = (job*) extractToQueue(pt->queue_threads);
	  	sem_post(pt->queue_mutex);
	  	my_job->func(my_job->args);
	  	free(my_job);
	  	pthread_mutex_lock(pt->cmutex);
	  	pt->n_working--;
	  	if (!pt->n_working)
	    	pthread_cond_signal(pt->idle);
	  	pthread_mutex_unlock(pt->cmutex);
		} 
		else 
		{
	  		sem_post(pt->queue_mutex);
	  		sem_wait(pt->st);
		}
	}
}

void poolSendJob(poolThread *pt, void *func, void *args)
{
	job *some_job = (job*) malloc(sizeof(job));
	some_job->func = func;
	some_job->args = args;
	some_job->next = NULL;
	sem_wait(pt->queue_mutex);
	addToQueue(pt->queue_threads, some_job);
	sem_post(pt->queue_mutex);
	sem_post(pt->st);
}

void poolWait(poolThread *pt) 
{
	pthread_mutex_lock(pt->cmutex);
	while (pt->n_working || pt->queue_threads->size) {
		pthread_cond_wait(pt->idle, pt->cmutex);
  	}
  	pthread_mutex_unlock(pt->cmutex);
}
