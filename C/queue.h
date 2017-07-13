#ifndef QUEUE_H_
#define QUEUE_H_

struct _job
{
	void (*func)(void *args);
	void *args;
	struct _job *next;
} typedef Job;

struct _queue
{
	Job *first;
	Job *last;
  	unsigned int size;
} typedef Queue;

Queue *newQueue();
void addToQueue(Queue *queue, void *task);
void *extractToQueue(Queue *queue);
void delToQueue(Queue *queue);

#endif