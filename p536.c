/**
 * MATTHEW RINNE
 * p536.c
 * CS300
 **/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>


#define MIN_PID 300 /* minimum pid */
#define MAX_PID 5000 /* maximum pid */


/********* functions *********/
int allocate_map(); /* allocates map of pids */
int allocate_pid(); /* allocates a pid, returns pid or -1 if unable */
void release_pid(int pid); /* reallocates a pid */
void *runner(); /* function to be run in threads */
/*****************************/

/****** global variable ******/
int *pid_map;
int counter;
pthread_mutex_t lock;
/*****************************/

int
allocate_map()
{
  pid_map = malloc(MAX_PID+1 * sizeof(int));

  for (int i=0; i<MAX_PID+1; i++) pid_map[i] = 0;

  if (pid_map)
  {
    printf("map successfully allocated\n");
    return 1;
  }
  else
  {
    printf("map unsuccessfully allocated\n");
    return 0;
  }
}

int
allocate_pid()
{
  for (int i=MIN_PID; i<MAX_PID+1; i++)
  {
    if (pid_map[i] == 0)
    {
      pid_map[i] = 1;
      printf("allocated a pid: %d\n", i);
      return i;
    }
  }
  return -1;
}

void
release_pid(int pid)
{
  pid_map[pid] = 0;
  printf("released a pid: %d\n", pid);
  return;
}

void *
runner()
{
  pthread_mutex_lock(&lock);
  int pid = allocate_pid();
  int sleep_time = rand() % 3;
  counter += 1;
  printf("Sleeping Time: %d secs; Thread ID = %d; Counter Value Incremented = %d\n",sleep_time,pid,counter);
  pthread_mutex_unlock(&lock);

  sleep(sleep_time);

  pthread_mutex_lock(&lock);
  counter -= 1;
  release_pid(pid);
  printf("Sleeping Time: %d secs; Thread ID = %d; Counter Value Decremented = %d\n",sleep_time,pid,counter);
  pthread_mutex_unlock(&lock);

  pthread_exit(0);
}

int
main(void)
{
  counter = 0;
  int NUM_THREADS;
  printf("Enter the number of threads you would like: \n");
  scanf("%d", &NUM_THREADS);

  pthread_t tid[NUM_THREADS];
  pthread_attr_t attr;

  allocate_map();
  srand(time(0));

  pthread_attr_init(&attr);

  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("\n mutex init has failed\n");
    return 1;
  }

  for (int i=0; i<NUM_THREADS; i++) pthread_create(&tid[i],&attr,runner,NULL);
  for (int i=0; i<NUM_THREADS; i++) pthread_join(tid[i], NULL);
  pthread_mutex_destroy(&lock);
  return 0;
}
