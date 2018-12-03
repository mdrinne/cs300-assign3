/**
 * MATTHEW RINNE
 * pp3.c
 * CS300
 **/

 #include <stdio.h>
 #include <stdlib.h>
 #include <pthread.h>
 #include <unistd.h>
 #include <time.h>
 #include <ctype.h>
 #include <string.h>
 #include <semaphore.h>

/***** buffer.h contents *****/
typedef int buffer_item;
#define BUFFER_SIZE 5
/*****************************/

/********* functions *********/
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer();
void *consumer();
/*****************************/

/****** global variables ******/
buffer_item buffer[BUFFER_SIZE];
pthread_mutex_t lock;
sem_t empty, full;
int in, out;
/******************************/

int
insert_item(buffer_item item)
{
  int value;
  sem_getvalue(&full,&value);

  /* buffer is full */
  if (value  == BUFFER_SIZE) return -1;

  buffer[in] = item;
  in = (in + 1) % BUFFER_SIZE;
  return 0;
}

int
remove_item(buffer_item *item)
{
  int value;
  sem_getvalue(&empty,&value);

  /* buffer is empty */
  if (value == BUFFER_SIZE) return -1;

  int temp;
  sem_getvalue(&full,&temp);
  *item = buffer[out];
  out = (out + 1) % BUFFER_SIZE;
  return 0;
}

void *
producer()
{
  buffer_item item;

  while(1)
  {
    sleep( (rand() % 5) + 1);
    printf("Producer thread ID: %d waiting\n", (int)pthread_self());
    sem_wait(&empty);

    item = rand();

    pthread_mutex_lock(&lock);
    if (insert_item(item)) fprintf(stderr,"Producer process error\n");
    else printf("Thread ID: %d produced %d\n", (int)pthread_self(), item);
    int value;
    sem_getvalue(&full,&value);
    value++;
    printf("Buffer size is %d\n", value);
    pthread_mutex_unlock(&lock);
    sem_post(&full);
    printf("Producer thread ID: %d signaled\n", (int)pthread_self());
  }

  pthread_exit(0);
}

void *
consumer()
{
  buffer_item item;

  while(1)
  {
    sleep( (rand() % 5) + 1);
    printf("Consumer thread ID: %d waiting\n", (int)pthread_self());
    sem_wait(&full);

    pthread_mutex_lock(&lock);
    if(remove_item(&item)) fprintf(stderr, "Consumer process error\n");
    else printf("Thread ID:%d consumed %d\n", (int)pthread_self(), item);
    int value;
    sem_getvalue(&full,&value);
    printf("Buffer size is %d\n", value);
    pthread_mutex_unlock(&lock);
    sem_post(&empty);
    printf("Consumer thread ID: %d signaled\n", (int)pthread_self());
  }

  pthread_exit(0);
}

int
main(int argc, char *argv[])
{
/* 1. Get command line arguments argv[1],argv[2],argv[3] */
  int sleep_time, pro_count, con_count;
  int flag = 0;
  in  = 0;
  out = 0;
  srand(time(0));

  if (argc < 4)
  {
    printf("Must include 3 integers in command line arguments\nexiting...\n");
    return 0;
  }

  char *temp1 = argv[1];
  char *temp2 = argv[2];
  char *temp3 = argv[3];

  for (int i=0; i<strlen(temp1); i++) if (isdigit(temp1[i]) == 0) flag = 1;
  for (int i=0; i<strlen(temp2); i++) if (isdigit(temp2[i]) == 0) flag = 1;
  for (int i=0; i<strlen(temp3); i++) if (isdigit(temp3[i]) == 0) flag = 1;

  if (flag == 1)
  {
    printf("Arguments must all be integers\nexiting...\n");
    return 0;
  }
  else
  {
    sleep_time = atoi(argv[1]);
    pro_count = atoi(argv[2]);
    con_count = atoi(argv[3]);
  }
/* 2. Initialize buffer */
  if (pthread_mutex_init(&lock, NULL) != 0)
  {
    printf("\n mutex init has failed\n");
    return 1;
  }

  pthread_t producers[pro_count];
  pthread_t consumers[con_count];
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  sem_init(&empty,0,BUFFER_SIZE);
  sem_init(&full,0,0);
/* 3. Create producer thread(s) */
  for (int i=0; i<pro_count; i++) pthread_create(&producers[i],&attr,producer,NULL);
/* 4. Create consumer thread(s) */
  for (int i=0; i<con_count; i++) pthread_create(&consumers[i],&attr,consumer,NULL);
/* 5. Sleep */
  sleep(sleep_time);
/* 6. Exit */
  sem_destroy(&empty);
  sem_destroy(&full);
  pthread_mutex_destroy(&lock);

  return 0;
}
