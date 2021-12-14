#ifndef THREAD_QUEUE_H
#define THREAD_QUEUE_H
#include <pthread.h>

/**
 * @brief Container for generic pointers that implements a FIFO queue-like
 * behaviour. The container is thread safe.
 * 
 */
typedef struct queue queue_t;
/**
 * @brief Struct that represents a queue.
 * 
 */
struct queue {
  /**
   * @brief Current size of the queue.
   * 
   */
  int size;

  /**
   * @brief Head node of the queue.
   * 
   */
  struct queue_node *head;

  /**
   * @brief Mutex for thread safe access to the queue.
   * 
   */
  pthread_mutex_t m;

  /**
   * @brief Flag to decide if the queue is locked.
   * 
   */
  char locked;
};

/**
 * @brief Struct that represents a queue node. It stores an arbitrary value on `job` and 
 * the next element of the queue.
 * 
 */
struct queue_node {
  /**
   * @brief Value stured on the queue.
   * 
   */
  void *job;

  /**
   * @brief Next element of the queue.
   * 
   */
  struct queue_node *next;
};

/**
 * @brief Deletes a node in the queue. 
 * 
 * @param node Node to be deleted.
 */
void queue_node_delete(struct queue_node *node);

/**
 * @brief Pushes a node to the queue, increasing its size.
 * It's recommended to run `queue_lock` before running this function.
 * 
 * @param q Queue to which the element will be pushed.
 * @param job Element that will be pushed onto the queue.
 * @return int 0 if fine, otherwise others.
 */
int queue_t_push(queue_t *q, void *job);

/**
 * @brief Removes the first element from the queue and returns its value.
 * It's recommended to run `queue_lock` before running this function.
 * 
 * @param q Queue from which the element will be popped.
 * @return void* Element that was popped from the queue.
 */
void *queue_t_pop(queue_t *q);

/**
 * @brief Deletes a queue and frees its memory.
 * 
 * @param q Queue to be deleted.
 */
void queue_t_delete(queue_t* q);

/**
 * @brief Allocates memory for a new queue node.
 * 
 * @return struct queue_node* Node that was created.
 */
struct queue_node *new_queue_node();

/**
 * @brief Creates a new queue with size 0 and a HEAD.
 * 
 * @return queue_t* Queue that was allocated.
 */
queue_t *new_queue();

/**
 * @brief Locks the queue for popping/pushing. This will impede other acceses to the
 * queue before running `queue_unlock`. This function will hang until the queue is
 * unlocked. Thread safe.
 * 
 * @param q 
 */
void queue_lock(queue_t *q);

/**
 * @brief Unlocks the queue for other threads to use. Throws when double unlocked.
 * 
 * @param q Queue to be unlocked.
 */
void queue_unlock(queue_t *q);

#endif
