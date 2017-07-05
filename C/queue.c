#include <stdlib.h>
#include "queue.h"


/* New Queue */
queue *newQueue()
{
  queue *new = (queue*) malloc(sizeof(queue));
  new->first = NULL;
  new->last  = NULL;
  new->size  = 0;
  return new;
}

/* Add job to queue */
void addToQueue(queue *some_queue, void *new)
{
  /*
  job *new = (job*) malloc(sizeof(job));
  new->func = task->func;
  new->args = task->args;
  new->next  = NULL;
  */
  if (some_queue->first == NULL) {
    some_queue->first = new;
    some_queue->last  = new;
  } else {
    (*some_queue->last).next = new;
    some_queue->last = new;
  }
  some_queue->size++;
}

/* Extract element from queue */
void *extractToQueue(queue *some_queue)
{
  job *tmp = some_queue->first;
  if (some_queue->last == tmp)
    some_queue->last = NULL;
  some_queue->first = tmp->next;
  some_queue->size--;
  return tmp;
}

/* Delete a job from queue */
void delToQueue(queue *some_queue)
{
  /* TODO
  job *act, *next;
  for(act = some_queue->first; act != NULL; act = next){
    next = act->next;
    free(act->value);
    free(act);
  }
  free(some_queue);
  */
}