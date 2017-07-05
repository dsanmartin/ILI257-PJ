#ifndef QUEUE_H_
#define QUEUE_H_

struct _job
{
	void (*func)(void* args);
	void *args;
	struct _job *next;
} typedef job;

struct _queue
{
	job *first;
	job *last;
  	unsigned int size;
} typedef queue;

queue *newQueue();
void addToQueue(queue *some_queue, void *task);
void *extractToQueue(queue *some_queue);
void delToQueue(queue *some_queue);

#endif