// C implementation for mandelbrot set fractals using libgraph, a simple
// library similar to the old Turbo C graphics library.
/*****************************************************************
 * ATENCAO: a biblioteca libgraph nao funciona com pthreads, entao
 * a solucao do exercicio nao deve chamar as funcoes graficas
 * NADA SERA EXIBIDO, INFELIZMENTE. :-(
 *****************************************************************/

#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logs.h"
#include "mandelbrot.h"
#include "queue.h"
#include "threading.h"

int empty_queue = 0;
queue_t *q, *results;

sem_t ssem, osem;

FILE *input;
int color_pick = 5;

float jobs_per_worker[N_THREADS] = {0};

void *worker_thread(void *param)
{
  // converting parameters to the known struct
  thread_info_t *info = (thread_info_t *)param;

  struct job_report *report;
  INFO("worker %d: initializing worker with id %d", info->id, info->id);

  clock_t start = clock();
  while (1)
  {
    report = malloc(sizeof(struct job_report));
    queue_lock(q);
    if (q->size <= N_THREADS)
    {
      sem_post(&ssem);
    }
    // if there are no jobs, increase the number of times there were no jobs
    if (!q->size)
    {
      empty_queue++;
      DEBUG("worker %d: waiting for queue to contain job", info->id);
      queue_unlock(q);
      sem_wait(&osem);
      queue_lock(q);
    }

    DEBUG("worker %d: popping queue", info->id);
    fractal_param_t *job = queue_t_pop(q);
    if (!job)
    {

      break;
    }

    jobs_per_worker[info->id]++;
    queue_unlock(q);
    fractal(job);
    free(job);

    report->ms = ((double)(clock() - start)) / CLOCKS_PER_SEC;
    report->worker = info->id;

    queue_lock(results);
    queue_t_push(results, report);
    queue_unlock(results);
  }
  DEBUG("worker %d: ended jobs, ending thread", info->id);
  queue_unlock(q);
  free(report);
  free(info);
  return NULL;
}

void *master_thread(void *_)
{
  fractal_param_t *p = malloc(sizeof(fractal_param_t));
  pthread_t pool[N_THREADS];
  thread_info_t *info;

  // starting worker threads
  for (int i = 0; i < N_THREADS; ++i)
  {
    info = malloc(sizeof(thread_info_t));
    info->id = i;
    pthread_create(pool + i, NULL, worker_thread, info);
  }

  while (input_params(p, input) != EOF)
  {
    INFO("master: adding job to the pool");
    sem_wait(&ssem);
    DEBUG("master: adding job to the queue");

    // acquiring queue lock and adding job to the queue
    queue_lock(q);
    queue_t_push(q, p);
    sem_post(&osem);
    queue_unlock(q);

    p = malloc(sizeof(fractal_param_t));
  }
  free(p);
  INFO("master: input ended, signalling workers");

  // adding NULL job descriptors to signalize end of work
  queue_lock(q);
  for (int i = 0; i < N_THREADS; ++i)
  {
    queue_t_push(q, NULL);
    sem_post(&osem);
  }
  queue_unlock(q);

  // waiting for all workers to join
  for (int i = 0; i < N_THREADS; ++i)
    pthread_join(pool[i], NULL);

  return NULL;
}

/****************************************************************
 * Essa versao do programa, sequencial le a descricao de um bloco
 * de imagem do conjunto de mandelbrot por vez e faz a geracao
 * e exibicao do mesmo na janela grafica. Na versao paralela com
 * pthreads, como indicado no enunciado, devem ser criados tres
 * tipos de threads: uma thread de entrada que le a descricao dos
 * blocos e alimenta uma fila para os trabalhadores, diversas
 * threads trabalhadoras que retiram um descritor de bloco da fila
 * e fazem o calculo da imagem e depositam um registro com os
 * dados sobre o processamento do bloco, como descrito no enunciado,
 * e uma thread final que recebe os registros sobre o processamento
 * e computa as estatisticas do sistema (que serao a unica saida
 * visivel do seu programa na versao que segue o enunciado.
 ****************************************************************/
int main(int argc, char *argv[])
{
  pthread_t io_t;

  // mutex and semaphores initialization
  sem_init(&ssem, 0, N_THREADS);
  sem_init(&osem, 0, 0);

  // initializing task and results queues
  q = new_queue();
  results = new_queue();

  if ((argc != 2) && (argc != 3))
    FATAL("usage %s filename [color_pick]\n", argv[0]);
  if (argc == 3)
    color_pick = atoi(argv[2]);
  if ((input = fopen(argv[1], "r")) == NULL)
    FATAL("fopen");

  // starting master thread
  pthread_create(&io_t, NULL, master_thread, NULL);
  // waiting for master thread to finish all jobs
  pthread_join(io_t, NULL);

  // calculating time mean, std and total

  double total_time = 0;
  struct queue_node *cur = results->head->next;
  while (cur->next != NULL)
  {
    total_time += ((struct job_report *)(cur->job))->ms;
    cur = cur->next;
  }
  double total_jobs = results->size;

  double std_time = 0;
  double mean_time = total_time / total_jobs;
  while (results->size)
  {
    struct job_report *res = queue_t_pop(results);
    double diff = res->ms - mean_time;
    std_time += diff * diff;
    free(res);
  }
  std_time = sqrt(std_time / total_jobs);

  // calculating job mean, std and total
  double std_job = 0;
  double mean_jobs = ((double)(total_jobs)) / N_THREADS;
  for (int i = 0; i < N_THREADS; ++i)
  {
    double diff = jobs_per_worker[i] - mean_jobs;
    std_job += diff * diff;
  }

  std_job = sqrt(std_job / N_THREADS);

  printf("Tarefas: total %d; media por trabalhador = %f(%f)\n", (int)total_jobs,
         mean_jobs, std_job);

  printf("Tempo médio por tarefa: %.6f (%.6f) ms\n", mean_time, std_job);
  printf("Fila estava vazia: %d vezes\n", empty_queue);

  // freeing memory
  sem_destroy(&osem);
  sem_destroy(&ssem);
  queue_t_delete(q);
  queue_t_delete(results);
  fclose(input);
  return 0;
}
