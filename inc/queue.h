#ifndef THREAD_QUEUE_H
#define THREAD_QUEUE_H
#include <pthread.h>

// params for each call to the fractal function
typedef struct {
  int left;
  int low; // lower left corner in the screen
  int ires;
  int jres; // resolution in pixels of the area to compute
  double xmin;
  double ymin; // lower left corner in domain (x,y)
  double xmax;
  double ymax; // upper right corner in domain (x,y)

} fractal_param_t;

typedef struct queue queue_t;
struct queue {
  int size;
  struct queue_node *head;
  pthread_mutex_t m;
  char locked;
};

struct queue_node {
  fractal_param_t *job;
  struct queue_node *next;
};

void queue_node_delete(struct queue_node *node);
int queue_t_push(queue_t *q, void *job);
void *queue_t_pop(queue_t *q);
void queue_t_delete(queue_t* q);
struct queue_node *new_queue_node();

queue_t *new_queue();

void queue_lock(queue_t *q);
void queue_unlock(queue_t *q);

#endif
