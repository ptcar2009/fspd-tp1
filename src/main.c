// C implementation for mandelbrot set fractals using libgraph, a simple
// library similar to the old Turbo C graphics library.
/*****************************************************************
 * ATENCAO: a biblioteca libgraph nao funciona com pthreads, entao
 * a solucao do exercicio nao deve chamar as funcoes graficas
 * NADA SERA EXIBIDO, INFELIZMENTE. :-(
 *****************************************************************/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logs.h"
#include <math.h>
#include "queue.h"

#ifndef N_THREADS
#define N_THREADS 9
#endif

#define MAXX 640
#define MAXY 480
#define MAXITER 32768

int empty_queue = 0;
queue_t *q, *results;

FILE *input;        // descriptor for the list of tiles (cannot be stdin)
int color_pick = 5; // decide how to choose the color palette

pthread_mutex_t mq, mr;
sem_t ssem, osem;

float jobs_per_worker[N_THREADS] = {0};

/****************************************************************
 * Nesta versao, o programa principal le diretamente da entrada
 * a descricao de cada quadrado que deve ser calculado; no EX1,
 * uma thread produtora deve ler essas descricoes sob demanda,
 * para manter uma fila de trabalho abastecida para as threads
 * trabalhadoras.
 ****************************************************************/
int input_params(fractal_param_t *p)
{
  int n;
  n = fscanf(input, "%d %d %d %d", &(p->left), &(p->low), &(p->ires),
             &(p->jres));
  if (n == EOF)
    return n;

  if (n != 4)
  {
    perror("fscanf(left,low,ires,jres)");
    exit(-1);
  }
  n = fscanf(input, "%lf %lf %lf %lf", &(p->xmin), &(p->ymin), &(p->xmax),
             &(p->ymax));
  if (n != 4)
  {
    perror("scanf(xmin,ymin,xmax,ymax)");
    exit(-1);
  }
  return 8;
}
struct job_report
{
  int worker;
  float ms;
};

typedef struct
{
  int id;
} thread_info_t;

/****************************************************************
 * A funcao a seguir faz a geracao dos blocos de imagem dentro
 * da area que vai ser trabalhada pelo programa. Para a versao
 * paralela, nao importa quais as dimensoes totais da area, basta
 * manter um controle de quais blocos estao sendo processados
 * a cada momento, para manter as restricoes desritas no enunciado.
 ****************************************************************/
// Function to draw mandelbrot set
void fractal(thread_info_t *info, fractal_param_t *p)
{
  double dx, dy;
  int i, j, k;
  double x, y, u, v, u2, v2;

  dx = (p->xmax - p->xmin) / p->ires;
  dy = (p->ymax - p->ymin) / p->jres;

  // scanning every point in that rectangular area.
  // Each point represents a Complex number (x + yi).
  // Iterate that complex number
  for (j = 0; j < p->jres; j++)
  {
    for (i = 0; i <= p->ires; i++)
    {
      x = i * dx + p->xmin; // c_real
      u = u2 = 0;           // z_real
      y = j * dy + p->ymin; // c_imaginary
      v = v2 = 0;           // z_imaginary

      // Calculate whether c(c_real + c_imaginary) belongs
      // to the Mandelbrot set or not and draw a pixel
      // at coordinates (i, j) accordingly
      // If you reach the Maximum number of iterations
      // and If the distance from the origin is
      // greater than 2 exit the loop
      for (k = 0; (k < MAXITER) && ((u2 + v2) < 4); ++k)
      {
        // Calculate Mandelbrot function
        // z = z*z + c where z is a complex number

        // imag = 2*z_real*z_imaginary + c_imaginary
        v = 2 * u * v + y;
        // real = z_real^2 - z_imaginary^2 + c_real
        u = u2 - v2 + x;
        u2 = u * u;
        v2 = v * v;
      }
    }
  }
}

int n_jobs;

void *ren_thread(void *param)
{
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
    if (!q->size)
    {
      empty_queue++;
      DEBUG("worker %d: waiting for queue to contain job", info->id);
      queue_unlock(q);
      sem_wait(&osem);
      queue_lock(q);
    }

    DEBUG("worker %d: popping queue", info->id);
    fractal_param_t *p = queue_t_pop(q);
    if (!p)
    {
      DEBUG("worker %d: ended jobs, ending thread", info->id);
      queue_unlock(q);
      free(report);
      free(info);
      return NULL;
    }

    jobs_per_worker[info->id]++;
    queue_unlock(q);
    fractal(info, p);
    free(p);

    report->ms = ((double)(clock() - start)) / CLOCKS_PER_SEC;
    report->worker = info->id;

    queue_lock(results);
    queue_t_push(results, report);
    queue_unlock(results);
  }
}

void *io_thread(void *_)
{
  fractal_param_t *p = malloc(sizeof(fractal_param_t));

  pthread_t pool[N_THREADS];
  // starting worker threads
  for (int i = 0; i < N_THREADS; ++i)
  {
    thread_info_t *info = malloc(sizeof(thread_info_t));
    info->id = i;
    pthread_create(pool + i, NULL, ren_thread, info);
  }

  while (input_params(p) != EOF)
  {
    INFO("master: adding job to the pool");
    sem_wait(&ssem);
    DEBUG("master: adding job to the queue");
    queue_lock(q);
    queue_t_push(q, p);
    sem_post(&osem);
    queue_unlock(q);
    p = malloc(sizeof(fractal_param_t));
  }
  free(p);
  INFO("master: input ended, signalling workers");

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
  pthread_mutex_init(&mq, NULL);
  pthread_mutex_init(&mr, NULL);
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
  pthread_create(&io_t, NULL, io_thread, NULL);

  pthread_join(io_t, NULL);

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
  pthread_mutex_destroy(&mq);
  pthread_mutex_destroy(&mr);
  fclose(input);
  return 0;
}
