#ifndef THREADING_C
#define THREADING_C

struct job_report
{
  int worker;
  float ms;
};

typedef struct
{
  int id;
} thread_info_t;

#ifndef N_THREADS
#define N_THREADS 9
#endif


#endif // THREADING_C
