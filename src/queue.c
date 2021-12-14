#include "queue.h"

#include <stdlib.h>

struct queue_node *new_queue_node()
{
  struct queue_node *node = malloc(sizeof(struct queue_node));
  node->job = NULL;
  node->next = NULL;
  return node;
}

void queue_node_delete(struct queue_node *node) { free(node); }

queue_t *new_queue()
{
  queue_t *q = malloc(sizeof(queue_t));
  q->size = 0;
  q->head = new_queue_node();
  q->locked = 0;
  pthread_mutex_init(&q->m, NULL);
  return q;
}

void queue_lock(queue_t *q)
{
  pthread_mutex_lock(&q->m);
  q->locked = 1;
}

void queue_unlock(queue_t *q)
{
  q->locked = 0;
  pthread_mutex_unlock(&q->m);
}

int queue_t_push(queue_t *q, void *job)
{
  struct queue_node *cur = q->head;

  while (cur->next != NULL)
    cur = cur->next;

  cur->next = new_queue_node();
  if (cur->next == NULL)
    return -1;

  cur->next->job = job;

  q->size++;
  return 0;
}

void *queue_t_pop(queue_t *q)
{
  if (q->size == 0)
    return NULL;

  void *job = q->head->next->job;
  struct queue_node *tmp = q->head->next;

  q->head->next = tmp->next;
  q->size--;

  queue_node_delete(tmp);

  return job;
}

void queue_t_delete(queue_t *q)
{
  while (q->size)
  {
    queue_t_pop(q);
  }
  queue_node_delete(q->head);
  pthread_mutex_destroy(&q->m);
  free(q);
}