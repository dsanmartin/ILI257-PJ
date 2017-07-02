#include <stdlib.h>
#include "threadpool.h"

poolThread *createPool(unsigned int n_threads) 
{
	poolThread *pt = (poolThread*) malloc(sizeof(poolThread));
	pt->threads = (pthread_t*) malloc(sizeof(pthread_t)*n_threads);
	pt->queue_threads = (queue*) malloc(sizeof(queue));
	pt->queue_threads->first = NULL;
	pt->queue_threads->last = NULL;
	pt->pool_size = n_threads;
	pt->n_working = 0;

	for(int i=0; i < n_threads; i++) 
		pthread_create(&(pt->threads[i]), NULL, _worker, pt);

	return pt;
}

void deletePool(poolThread *pt)
{
	// Deberia borrar los threads pero es mucho weveo
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

//void *_worker(void *vp); 

void poolSendJob(poolThread *pt, void *func, void *args)
{
	job *trabajo
}

void poolWait(poolThread *pt) 
{

}
