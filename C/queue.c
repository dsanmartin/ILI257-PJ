#include <stdlib.h>
#include "queue.h"


/* New Queue */
Queue *newQueue()
{
  Queue *new = (Queue*) malloc(sizeof(Queue));
  new->first = NULL;
  new->last  = NULL;
  new->size  = 0;
  return new;
}

/* Add job to queue */
void addToQueue(Queue *queue, void *task)
{
  if (queue->first == NULL) {
    queue->first = task;
    queue->last  = task;
  } 
  else 
  {
    (*queue->last).next = task;
    queue->last = task;
  }
  queue->size++;
}

/* Extract element from queue */
void *extractToQueue(Queue *queue)
{
  Job *tmp = queue->first;
  if (queue->last == tmp)
    queue->last = NULL;
  queue->first = tmp->next;
  queue->size--;
  return tmp;
}

/* Delete a job from queue */
void delToQueue(Queue *queue)
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